import sys
import logging
from PyQt5 import QtWidgets
from PyQt5.QtGui import QColor


class ColorHandler(logging.StreamHandler):
    def __init__(self, _text_edit):
        super(ColorHandler, self).__init__()
        self.text_edit = _text_edit
        assert (isinstance(self.text_edit, QtWidgets.QTextEdit))
        self.ansi_colors = dict(
            black=30,
            red=31,
            green=32,
            yellow=33,
            blue=34,
            magenta=35,
            cyan=36,
            white=37
        )
        self.rgb_colors = dict(
            black=QColor(0, 0, 0),
            red=QColor(255, 0, 0),
            green=QColor(0, 255, 0),
            yellow=QColor(255, 255, 0),
            blue=QColor(0, 0, 128),
            magenta=QColor(255, 0, 255),
            cyan=QColor(0, 255, 255),
            white=QColor(255, 255, 255)
        )

    def emit(self, record):
        msg_colors = {
            logging.DEBUG: "green",
            logging.INFO: "cyan",
            logging.WARNING: "yellow",
            logging.ERROR: "red"
        }
        color_id = msg_colors.get(record.levelno, "blue")
        self.stream.write('\x1b[%s;1m%s\x1b[0m\n' % (self.ansi_colors[color_id], record))
        if self.text_edit:
            self.text_edit.setTextColor(QColor(255, 255, 255))
            self.text_edit.insertPlainText("%s:%d : " % (record.funcName, record.lineno))
            self.text_edit.setTextColor(self.rgb_colors[color_id])
            self.text_edit.insertPlainText("%s\n" % (record.getMessage()))
