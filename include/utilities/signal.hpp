/// @file signal
/// @brief
/// @date 8/20/19
/// @author enrico

#pragma once

#include <functional>
#include <map>

/// @brief
/// @tparam Args
template <typename... Args> class Signal {
public:
	Signal() : current_id(0)
	{
	}

	// copy creates new signal
	Signal(Signal const &other) : current_id(0)
	{
	}

	// connects a member function to this Signal
	template <typename T> size_t connect_member(T *inst, void (T::*func)(Args...))
	{
		return connect([=](Args... args) { (inst->*func)(args...); });
	}

	// connects a const member function to this Signal
	template <typename T> size_t connect_member(T *inst, void (T::*func)(Args...) const)
	{
		return connect([=](Args... args) { (inst->*func)(args...); });
	}

	// connects a std::function to the signal. The returned
	// value can be used to disconnect the function again
	size_t connect(std::function<void(Args...)> const &slot) const
	{
		slots.insert(std::make_pair(current_id, slot));
		return current_id++;
	}

	// disconnects a previously connected function
	void disconnect(size_t id) const
	{
		slots.erase(id);
	}

	// disconnects all previously connected functions
	void disconnect_all() const
	{
		slots.clear();
	}

	// calls all connected functions
	void emit(Args... p)
	{
		for (auto const &it : slots)
			it.second(std::forward<Args>(p)...);
	}

	// assignment creates new Signal
	Signal &operator=(Signal const &other)
	{
		disconnect_all();
	}

private:
	mutable std::map<size_t, std::function<void(Args...)> > slots;
	mutable size_t current_id;
};
