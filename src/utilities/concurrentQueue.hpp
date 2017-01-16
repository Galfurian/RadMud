/// @file   concurrentQueue.hpp
/// @author Enrico Fraccaroli
/// @date   Jan 16 2017
/// @copyright
/// Copyright (c) 2017 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
/// Permission is hereby granted, free of charge, to any person obtaining a
/// copy of this software and associated documentation files (the "Software"),
/// to deal in the Software without restriction, including without limitation
/// the rights to use, copy, modify, merge, publish, distribute, sublicense,
/// and/or sell copies of the Software, and to permit persons to whom the
/// Software is furnished to do so, subject to the following conditions:
///     The above copyright notice and this permission notice shall be included
///     in all copies or substantial portions of the Software.
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
/// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
/// DEALINGS IN THE SOFTWARE.

#pragma once

#include "logger.hpp"

#include <queue>
#include <list>
#include <mutex>
#include <thread>
#include <cstdint>
#include <condition_variable>

/** A thread-safe asynchronous queue */
template<class T, class Container = std::list<T>>
class ConcurrentQueue
{
    typedef typename Container::value_type value_type;
    typedef typename Container::size_type size_type;
    typedef Container container_type;

public:

    /*! Create safe queue. */
    ConcurrentQueue() = default;

    ConcurrentQueue(ConcurrentQueue && sq)
    {
        m_queue = std::move(sq.m_queue);
    }

    ConcurrentQueue(const ConcurrentQueue & sq)
    {
        std::lock_guard<std::mutex> lock(sq.m_mutex);
        m_queue = sq.m_queue;
    }

    /*! Destroy safe queue. */
    ~ConcurrentQueue()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
    }

    /**
     * Sets the maximum number of items in the queue. Defaults is 0: No limit
     * \param[in] item An item.
     */
    void set_max_num_items(unsigned int max_num_items)
    {
        m_max_num_items = max_num_items;
    }

    value_type & front()
    {
//        Logger::log(LogLevel::Debug, "ConcurrentQueue::front()");
        std::lock_guard<std::mutex> lock(m_mutex);
//        m_condition.wait(lock, [this]()
//        {
//            return !m_queue.empty();
//        });
        return m_queue.front();
    }

    /**
     *  Pushes the item into the queue.
     * \param[in] item An item.
     * \return true if an item was pushed into the queue
     */
    bool push(const value_type & item)
    {
        Logger::log(LogLevel::Debug,
                    "ConcurrentQueue::push(const value_type & item)");
        std::lock_guard<std::mutex> lock(m_mutex);
        if ((m_max_num_items > 0) &&
            (m_queue.size() > m_max_num_items))
        {
            return false;
        }
        m_queue.push(item);
        m_condition.notify_one();
        return true;
    }

    /**
     *  Pushes the item into the queue.
     * \param[in] item An item.
     * \return true if an item was pushed into the queue
     */
    bool push(const value_type && item)
    {
        Logger::log(LogLevel::Debug,
                    "ConcurrentQueue::push(const value_type && item)");
        std::lock_guard<std::mutex> lock(m_mutex);
        if ((m_max_num_items > 0) &&
            (m_queue.size() > m_max_num_items))
        {
            return false;
        }
        m_queue.push(item);
        m_condition.notify_one();
        return true;
    }

    /**
     *  Pops item from the queue.
     *  If queue is empty, this function blocks until item becomes available.
     * \param[out] item The item.
     */
    void pop(value_type & item)
    {
        Logger::log(LogLevel::Debug, "ConcurrentQueue::pop(value_type & item)");
        std::lock_guard<std::mutex> lock(m_mutex);
//        m_condition.wait(lock, [this]()
//        {
//            return !m_queue.empty();
//        });
        item = m_queue.front();
        m_queue.pop();
    }

    /**
     *  Pops item from the queue.
     *  If queue is empty, this function blocks until item becomes available.
     * \param[out] item The item.
     */
    void pop()
    {
        Logger::log(LogLevel::Debug, "ConcurrentQueue::pop()");
        std::lock_guard<std::mutex> lock(m_mutex);
//        m_condition.wait(lock, [this]()
//        {
//            return !m_queue.empty();
//        });
        m_queue.pop();
    }

    /**
     *  Pops item from the queue using the contained type's move assignment operator, if it has one..
     *  This method is identical to the pop() method if that type has no move assignment operator.
     *  If queue is empty, this function blocks until item becomes available.
     * \param[out] item The item.
     */
    void move_pop(value_type & item)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
//        m_condition.wait(lock, [this]() // Lambda funct
//        {
//            return !m_queue.empty();
//        });
        item = std::move(m_queue.front());
        m_queue.pop();
    }

    /**
     *  Tries to pop item from the queue.
     * \param[out] item The item.
     * \return False is returned if no item is available.
     */
    bool try_pop(value_type & item)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_queue.empty())
            return false;

        item = m_queue.front();
        m_queue.pop();
        return true;
    }

    /**
     *  Tries to pop item from the queue using the contained type's move assignment operator, if it has one..
     *  This method is identical to the try_pop() method if that type has no move assignment operator.
     * \param[out] item The item.
     * \return False is returned if no item is available.
     */
    bool try_move_pop(value_type & item)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_queue.empty())
            return false;

        item = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }

    /**
     *  Pops item from the queue. If the queue is empty, blocks for timeout microseconds, or until item becomes available.
     * \param[out] t An item.
     * \param[in] timeout The number of microseconds to wait.
     * \return true if get an item from the queue, false if no item is received before the timeout.
     */
    bool timeout_pop(value_type & item, std::uint64_t timeout)
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        if (m_queue.empty())
        {
            if (timeout == 0)
                return false;

            if (m_condition.wait_for(lock,
                                     std::chrono::microseconds(timeout)) ==
                std::cv_status::timeout)
                return false;
        }

        item = m_queue.front();
        m_queue.pop();
        return true;
    }

    /**
     *  Pops item from the queue using the contained type's move assignment operator, if it has one..
     *  If the queue is empty, blocks for timeout microseconds, or until item becomes available.
     *  This method is identical to the try_pop() method if that type has no move assignment operator.
     * \param[out] t An item.
     * \param[in] timeout The number of microseconds to wait.
     * \return true if get an item from the queue, false if no item is received before the timeout.
     */
    bool timeout_move_pop(value_type & item, std::uint64_t timeout)
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        if (m_queue.empty())
        {
            if (timeout == 0)
                return false;

            if (m_condition.wait_for(lock,
                                     std::chrono::microseconds(timeout)) ==
                std::cv_status::timeout)
                return false;
        }

        item = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }

    /**
     *  Gets the number of items in the queue.
     * \return Number of items in the queue.
     */
    size_type size() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

    /**
     *  Check if the queue is empty.
     * \return true if queue is empty.
     */
    bool empty() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    /**
     *  Swaps the contents.
     * \param[out] sq The ConcurrentQueue to swap with 'this'.
     */
    void swap(ConcurrentQueue & sq)
    {
        if (this != &sq)
        {
            std::lock_guard<std::mutex> lock1(m_mutex);
            std::lock_guard<std::mutex> lock2(sq.m_mutex);
            m_queue.swap(sq.m_queue);

            if (!m_queue.empty())
                m_condition.notify_all();

            if (!sq.m_queue.empty())
                sq.m_condition.notify_all();
        }
    }

    /*! The copy assignment operator */
    ConcurrentQueue & operator=(const ConcurrentQueue & sq)
    {
        if (this != &sq)
        {
            std::lock_guard<std::mutex> lock1(m_mutex);
            std::lock_guard<std::mutex> lock2(sq.m_mutex);
            std::queue<T, Container> temp{sq.m_queue};
            m_queue.swap(temp);

            if (!m_queue.empty())
                m_condition.notify_all();
        }

        return *this;
    }

    /*! The move assignment operator */
    ConcurrentQueue & operator=(ConcurrentQueue && sq)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue = std::move(sq.m_queue);

        if (!m_queue.empty()) m_condition.notify_all();

        return *this;
    }


private:

    std::queue<T, Container> m_queue;
    mutable std::mutex m_mutex;
    std::condition_variable m_condition;
    unsigned int m_max_num_items = 0;
};