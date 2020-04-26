# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'radmuddbe.ui'
#
# Created by: PyQt5 UI code generator 5.13.0
#
# WARNING! All changes made in this file will be lost!


import gc
import logging
import sys
from PyQt5 import QtCore, QtGui, QtWidgets, QtSql
from PyQt5.QtGui import QColor
from functools import partial
from qtmodern import styles
from radmuddbe_gui import *


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
            self.text_edit.ensureCursorVisible()


class RadMudEditor(QtWidgets.QMainWindow, Ui_MainWindow):
    def __init__(self):
        super(RadMudEditor, self).__init__()

        # === DATABASE ========================================================
        self.db_filename = ""
        self.db = QtSql.QSqlDatabase.addDatabase('QSQLITE')

        # === MODELS ==========================================================
        # MODEL
        self.mdl_model = None
        self.mdl_model_body_part = None
        # MATERIAL
        self.mdl_material = None
        # BODYPART
        self.mdl_bodypart = None
        self.mdl_bodypart_resources = None
        self.mdl_bodypart_weapon = None
        # MOBILE
        self.mdl_mobile = None
        self.mdl_mobile_spawn = None
        # RACE
        self.mdl_race = None
        self.mdl_race_skill = None
        self.mdl_race_body_part = None
        # SKILL
        self.mdl_skill = None
        self.mdl_skill_prerequisite = None
        self.mdl_skill_knowledge = None
        self.mdl_skill_combat_modifier = None
        self.mdl_skill_ability_modifier = None
        self.mdl_skill_status_modifier = None
        # PRODUCTION
        self.mdl_production = None
        self.mdl_production_ingredient = None
        self.mdl_production_tool = None
        self.mdl_production_knowledge = None
        self.mdl_production_outcome = None
        # PROFESSION
        self.mdl_profession = None
        # BUILDING
        self.mdl_building = None
        self.mdl_building_ingredient = None
        self.mdl_building_knowledge = None
        self.mdl_building_tool = None
        # CURRENCY
        self.mdl_currency = None
        # AREA
        self.mdl_area = None
        self.mdl_area_list = None
        self.mdl_room = None
        # PLAYER
        self.mdl_player = None
        self.mdl_player_skill = None
        self.mdl_player_item = None
        self.mdl_player_variable = None
        # ITEM
        self.mdl_item = None
        self.mdl_item_content = None
        self.mdl_item_liquid_content = None
        self.mdl_item_player = None
        self.mdl_item_room = None
        # SHOP
        self.mdl_shop = None
        self.mdl_shop_default_stock = None

        self.shortcut = QtWidgets.QShortcut(QtGui.QKeySequence("F5"), self)
        self.shortcut.activated.connect(self.update_models)

        self.shortcut = QtWidgets.QShortcut(QtGui.QKeySequence("Esc"), self)
        self.shortcut.activated.connect(self.clear_filters)

        # === SUPPORT VARIABLES ===============================================
        self.item_player_pindex = QtCore.QModelIndex()

        # === UI SETUP ========================================================
        self.setupUi(self)
        self.actionOpen.triggered.connect(self.on_click_open_database)
        self.actionClose.triggered.connect(self.close_database)
        self.actionSave.triggered.connect(self.save_database)
        self.actionExit.triggered.connect(self.close_application)

        # Prepare all the table.
        for obj in gc.get_objects():
            if isinstance(obj, QtWidgets.QTableView):
                # obj.setSizeAdjustPolicy(QtWidgets.QAbstractScrollArea.AdjustToContents)
                # obj.resizeColumnsToContents()
                obj.setSelectionMode(QtWidgets.QAbstractItemView.SingleSelection)
                obj.setSelectionBehavior(QtWidgets.QAbstractItemView.SelectRows)
                obj.setItemDelegate(QtSql.QSqlRelationalDelegate(obj))
                obj.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
                obj.customContextMenuRequested.connect(self.table_menu)

        self.log_window.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        self.log_window.customContextMenuRequested.connect(self.log_menu)

        logging.getLogger().setLevel(logging.DEBUG)
        logging.getLogger().addHandler(ColorHandler(self.log_window))

        # === OPEN DATABASE CONNECTION ========================================
        self.open_database("../system/radmud.db")

    def table_menu(self, position):
        widget = self.sender()
        if not isinstance(widget, QtWidgets.QTableView):
            logging.error("Sender is not QtWidgets.QTableView.")
            return
        try:
            index = widget.indexAt(position)
        except Exception as err:
            logging.error("Cannot get item at the given position.")
            logging.error(str(err))
            return
        menu = QtWidgets.QMenu("Menu", self)
        menu.addAction("Insert", partial(self.table_action_insert, table=widget))
        menu.addAction("Delete", partial(self.table_action_delete, table=widget, index=index))
        # Show the context menu.
        menu.exec_(QtGui.QCursor.pos())

    def log_menu(self, position):
        widget = self.sender()
        if not isinstance(widget, QtWidgets.QTextEdit):
            logging.error("Sender is not QtWidgets.QTextEdit.")
            return False
        menu = QtWidgets.QMenu("Menu", self)
        menu.addAction("Select All", lambda: self.log_window.selectAll())
        menu.addAction("Copy", lambda: self.log_window.copy())
        menu.addSeparator()
        menu.addAction("Clear", lambda: self.log_window.clear())
        # Show the context menu.
        menu.exec_(QtGui.QCursor.pos())
        return True

    @staticmethod
    def table_action_insert(table):
        table.model().insertRow(table.model().rowCount())
        table.scrollToBottom()

    def table_action_delete(self, table, index):
        if not isinstance(table, QtWidgets.QTableView):
            logging.error("Table is not a QtWidgets.QTableView.")
            return False
        query = "DELETE FROM {} WHERE _rowid_ = {}".format(table.model().tableName(), index.row() + 1)
        if self.db.exec(query):
            if table.model().submitAll():
                if table.model().select():
                    logging.info("Removed row index '{}'.".format(index.row()))
                    return True
        logging.error(self.db.lastError())
        return False

    def close_application(self):
        self.close_database()
        exit(0)

    def open_database(self, filename):
        if not filename:
            logging.error("You need to provide a filename!")
            return False
        logging.info("Opening database '%s'..." % filename)
        if self.db.isOpen():
            logging.error("You are already connected to database '%s'" % filename)
            return False
        self.db.setDatabaseName(filename)
        if not self.db.open():
            logging.error("Cannot connect to database '%s'" % filename)
            return False
        self.db_filename = filename
        self.setup_tables()
        # self.db.transaction()
        logging.info("Opening database '%s'... done." % filename)
        return True

    def save_database(self):
        logging.info("Saving database '%s'..." % self.db_filename)
        if not self.db.isOpen():
            logging.error("You are not connected to any database!")
            return False
        for obj in gc.get_objects():
            if isinstance(obj, QtSql.QSqlRelationalTableModel):
                if not obj.submitAll():
                    logging.error(self.db.lastError())
        logging.info("Saving database '%s'... done." % self.db_filename)
        return True

    def update_models(self):
        logging.info("Updating models...")
        for obj in gc.get_objects():
            if isinstance(obj, QtWidgets.QTableView):
                obj.horizontalHeader().setMaximumSectionSize(500)
                obj.model().select()
                obj.resizeColumnsToContents()
        logging.info("Updating models... done.")

    def clear_filters(self):
        logging.info("Clearing filters...")
        for obj in gc.get_objects():
            if isinstance(obj, QtSql.QSqlRelationalTableModel):
                obj.setFilter("")
            if isinstance(obj, QtWidgets.QTableView):
                obj.clearSelection()
        logging.info("Clearing filters... done.")

    def close_database(self):
        logging.info("Closing database '%s'..." % self.db_filename)
        if not self.db.isOpen():
            logging.error("The database is already closed!")
            return False
        self.db.close()
        if self.db.isOpen():
            logging.error("You failed to close the database '%s'!" % self.db_filename)
            return False
        self.update_models()
        logging.info("Closing database '%s'... done." % self.db_filename)
        return True

    def on_click_open_database(self):
        options = QtWidgets.QFileDialog.Options()
        options |= QtWidgets.QFileDialog.DontUseNativeDialog
        filename, _ = QtWidgets.QFileDialog.getOpenFileName(self, "Open database", "",
                                                            "Database Files (*.db);;All Files (*)", options=options)
        self.open_database(filename)

    def update_status_bar(self, msg):
        self.statusBar().showMessage(msg)

    @staticmethod
    def init_model(name):
        model = QtSql.QSqlRelationalTableModel()
        model.setTable(name)
        model.setEditStrategy(QtSql.QSqlRelationalTableModel.OnManualSubmit)
        return model

    @staticmethod
    def set_relation(model, field_name, tbl_name, tbl_index, tbl_display):
        index = model.fieldIndex(field_name)
        model.setHeaderData(index, QtCore.Qt.Horizontal, field_name)
        model.setRelation(index, QtSql.QSqlRelation(tbl_name, tbl_index, tbl_display))

    def setup_tables(self):
        # === MODEL ===========================================================
        self.mdl_model = self.init_model('model')
        self.set_relation(self.mdl_model, "type", "ModelType", "id", "name")
        self.set_relation(self.mdl_model, "material", "MaterialType", "id", "name")
        self.tbl_model.setModel(self.mdl_model)
        self.tbl_model.clicked.connect(self.model_clicked)

        self.mdl_model_body_part = self.init_model('modelbodypart')
        self.set_relation(self.mdl_model_body_part, "model", "model", "vnum", "name")
        self.set_relation(self.mdl_model_body_part, "bodypart", "bodypart", "vnum", "name")
        self.tbl_model_body_part.setModel(self.mdl_model_body_part)

        # === MATERIAL ========================================================
        self.mdl_material = self.init_model('material')
        self.set_relation(self.mdl_material, "type", "MaterialType", "id", "name")
        self.tbl_material.setModel(self.mdl_material)

        # === CURRENCY ========================================================
        self.mdl_currency = self.init_model('currency')
        self.set_relation(self.mdl_currency, "model", "model", "vnum", "name")
        self.set_relation(self.mdl_currency, "material", "material", "vnum", "name")
        self.tbl_currency.setModel(self.mdl_currency)

        # === BODYPART ========================================================
        self.mdl_bodypart = self.init_model('BodyPart')
        self.tbl_bodypart.setModel(self.mdl_bodypart)
        self.tbl_bodypart.clicked.connect(self.bodypart_clicked)

        self.mdl_bodypart_resources = self.init_model('BodyPartResources')
        self.set_relation(self.mdl_bodypart_resources, "bodyPart", "bodypart", "vnum", "name")
        self.set_relation(self.mdl_bodypart_resources, "resourceModel", "model", "vnum", "name")
        self.set_relation(self.mdl_bodypart_resources, "material", "material", "vnum", "name")
        self.tbl_bodypart_resources.setModel(self.mdl_bodypart_resources)

        self.mdl_bodypart_weapon = self.init_model('BodyPartWeapon')
        self.set_relation(self.mdl_bodypart_weapon, "bodypart", "BodyPart", "vnum", "name")
        self.tbl_bodypart_weapon.setModel(self.mdl_bodypart_weapon)

        # === BUILDING ========================================================
        self.mdl_building = self.init_model('building')
        self.set_relation(self.mdl_building, "model", "model", "vnum", "name")
        self.tbl_building.setModel(self.mdl_building)
        self.tbl_building.clicked.connect(self.building_clicked)

        self.mdl_building_ingredient = self.init_model('BuildingIngredient')
        self.set_relation(self.mdl_building_ingredient, "building", "building", "vnum", "name")
        self.set_relation(self.mdl_building_ingredient, "resource", "resourcetype", "id", "name")
        self.tbl_building_ingredient.setModel(self.mdl_building_ingredient)

        self.mdl_building_knowledge = self.init_model('BuildingKnowledge')
        self.set_relation(self.mdl_building_knowledge, "building", "building", "vnum", "name")
        self.set_relation(self.mdl_building_knowledge, "knowledge", "knowledge", "id", "name")
        self.tbl_building_knowledge.setModel(self.mdl_building_knowledge)

        self.mdl_building_tool = self.init_model('buildingtool')
        self.set_relation(self.mdl_building_tool, "building", "building", "vnum", "name")
        self.set_relation(self.mdl_building_tool, "tool", "tooltype", "id", "name")
        self.tbl_building_tool.setModel(self.mdl_building_tool)

        # === MOBILE ==========================================================
        self.mdl_mobile = self.init_model('mobilemodel')
        self.set_relation(self.mdl_mobile, "race", "race", "vnum", "name")
        self.set_relation(self.mdl_mobile, "faction", "faction", "vnum", "name")
        self.tbl_mobile.setModel(self.mdl_mobile)
        self.tbl_mobile.clicked.connect(self.mobile_clicked)

        self.mdl_mobile_spawn = self.init_model('mobilespawn')
        self.set_relation(self.mdl_mobile_spawn, "mobile", "mobilemodel", "vnum", "propnoun")
        self.tbl_mobile_spawn.setModel(self.mdl_mobile_spawn)

        # === RACE ============================================================
        self.mdl_race = self.init_model('race')
        self.tbl_race.setModel(self.mdl_race)
        self.tbl_race.clicked.connect(self.race_clicked)

        self.mdl_race_skill = self.init_model('racebaseskill')
        self.set_relation(self.mdl_race_skill, "race", "race", "vnum", "name")
        self.set_relation(self.mdl_race_skill, "skill", "skill", "vnum", "name")
        self.tbl_race_skill.setModel(self.mdl_race_skill)

        self.mdl_race_body_part = self.init_model('racebodypart')
        self.set_relation(self.mdl_race_body_part, "race", "race", "vnum", "name")
        self.set_relation(self.mdl_race_body_part, "bodypart", "bodypart", "vnum", "name")
        self.tbl_race_body_part.setModel(self.mdl_race_body_part)

        # === PRODUCTION ======================================================
        self.mdl_production = self.init_model('production')
        self.set_relation(self.mdl_production, "profession", "profession", "vnum", "command")
        self.set_relation(self.mdl_production, "workbench", "tooltype", "id", "name")
        self.tbl_production.setModel(self.mdl_production)
        self.tbl_production.clicked.connect(self.production_clicked)

        self.mdl_production_ingredient = self.init_model('productioningredient')
        self.set_relation(self.mdl_production_ingredient, "production", "production", "vnum", "name")
        self.set_relation(self.mdl_production_ingredient, "ingredient", "resourcetype", "id", "name")
        self.tbl_production_ingredient.setModel(self.mdl_production_ingredient)

        self.mdl_production_tool = self.init_model('productiontool')
        self.set_relation(self.mdl_production_tool, "production", "production", "vnum", "name")
        self.set_relation(self.mdl_production_tool, "tool", "tooltype", "id", "name")
        self.tbl_production_tool.setModel(self.mdl_production_tool)

        self.mdl_production_knowledge = self.init_model('productionknowledge')
        self.set_relation(self.mdl_production_knowledge, "production", "production", "vnum", "name")
        self.set_relation(self.mdl_production_knowledge, "knowledge", "knowledge", "id", "name")
        self.tbl_production_knowledge.setModel(self.mdl_production_knowledge)

        self.mdl_production_outcome = self.init_model('productionoutcome')
        self.set_relation(self.mdl_production_outcome, "production", "production", "vnum", "name")
        self.set_relation(self.mdl_production_outcome, "outcome", "model", "vnum", "name")
        self.tbl_production_outcome.setModel(self.mdl_production_outcome)

        # === PROFESSION ======================================================
        self.mdl_profession = self.init_model('profession')
        self.tbl_profession.setModel(self.mdl_profession)

        # === SKILL ===========================================================
        self.mdl_skill = self.init_model('skill')
        self.set_relation(self.mdl_skill, "ability", "abilitytype", "id", "name")
        self.tbl_skill.setModel(self.mdl_skill)
        self.tbl_skill.clicked.connect(self.skill_clicked)

        self.mdl_skill_prerequisite = self.init_model('skillprerequisite')
        self.set_relation(self.mdl_skill_prerequisite, "skill", "skill", "vnum", "name")
        self.set_relation(self.mdl_skill_prerequisite, "requiredskill", "skill", "vnum", "name")
        self.tbl_skill_prerequisite.setModel(self.mdl_skill_prerequisite)

        self.mdl_skill_knowledge = self.init_model('skillknowledge')
        self.set_relation(self.mdl_skill_knowledge, "skill", "skill", "vnum", "name")
        self.set_relation(self.mdl_skill_knowledge, "knowledge", "knowledge", "id", "name")
        self.tbl_skill_knowledge.setModel(self.mdl_skill_knowledge)

        self.mdl_skill_combat_modifier = self.init_model('skillcombatmodifier')
        self.set_relation(self.mdl_skill_combat_modifier, "skill", "skill", "vnum", "name")
        self.set_relation(self.mdl_skill_combat_modifier, "modifier", "combatmodifier", "id", "name")
        self.tbl_skill_combat_modifier.setModel(self.mdl_skill_combat_modifier)

        self.mdl_skill_ability_modifier = self.init_model('skillabilitymodifier')
        self.set_relation(self.mdl_skill_ability_modifier, "skill", "skill", "vnum", "name")
        self.set_relation(self.mdl_skill_ability_modifier, "ability", "abilitytype", "id", "name")
        self.tbl_skill_ability_modifier.setModel(self.mdl_skill_ability_modifier)

        self.mdl_skill_status_modifier = self.init_model('skillstatusmodifier')
        self.set_relation(self.mdl_skill_status_modifier, "skill", "skill", "vnum", "name")
        self.set_relation(self.mdl_skill_status_modifier, "status", "statusmodifier", "id", "name")
        self.tbl_skill_status_modifier.setModel(self.mdl_skill_status_modifier)

        # === AREA ============================================================
        self.mdl_area = self.init_model('area')
        self.tbl_area.setModel(self.mdl_area)
        self.tbl_area.clicked.connect(self.area_clicked)

        self.mdl_area_list = self.init_model('arealist')
        self.set_relation(self.mdl_area_list, "area", "area", "vnum", "name")
        self.tbl_area_list.setModel(self.mdl_area_list)

        self.mdl_room = self.init_model('room')
        self.set_relation(self.mdl_room, "terrain", "terrain", "vnum", "name")
        self.tbl_room.setModel(self.mdl_room)

        # === PLAYER ==========================================================
        self.mdl_player = self.init_model('player')
        self.tbl_player.setModel(self.mdl_player)
        self.tbl_player.clicked.connect(self.player_clicked)

        self.mdl_player_skill = self.init_model('playerskill')
        self.set_relation(self.mdl_player_skill, "skill", "skill", "vnum", "name")
        self.tbl_player_skill.setModel(self.mdl_player_skill)

        self.mdl_player_item = self.init_model('itemplayer')
        self.set_relation(self.mdl_player_item, "position", "bodypart", "vnum", "name")
        self.tbl_player_item.setModel(self.mdl_player_item)
        self.tbl_player_item.setMouseTracking(True)
        self.tbl_player_item.mouseMoveEvent = self.item_player_mouse_move_event

        self.mdl_player_variable = self.init_model('playervariable')
        self.tbl_player_variable.setModel(self.mdl_player_variable)

        # === ITEM ============================================================
        self.mdl_item = self.init_model('item')
        self.set_relation(self.mdl_item, "model", "model", "vnum", "name")
        self.set_relation(self.mdl_item, "composition", "material", "vnum", "name")
        self.set_relation(self.mdl_item, "quality", "itemquality", "id", "name")
        self.tbl_item.setModel(self.mdl_item)
        self.tbl_item.clicked.connect(self.item_clicked)

        self.mdl_item_content = self.init_model('itemcontent')
        self.tbl_item_content.setModel(self.mdl_item_content)

        self.mdl_item_liquid_content = self.init_model('itemliquidcontent')
        self.set_relation(self.mdl_item_liquid_content, "content", "liquid", "vnum", "name")
        self.tbl_item_liquid_content.setModel(self.mdl_item_liquid_content)

        self.mdl_item_player = self.init_model('itemplayer')
        self.set_relation(self.mdl_item_player, "position", "bodypart", "vnum", "name")
        self.tbl_item_player.setModel(self.mdl_item_player)

        self.mdl_item_room = self.init_model('itemroom')
        self.tbl_item_room.setModel(self.mdl_item_room)

        # === SHOP ============================================================
        self.mdl_shop = self.init_model('shop')
        self.tbl_shop.setModel(self.mdl_shop)
        self.tbl_shop.clicked.connect(self.shop_clicked)

        self.mdl_shop_default_stock = self.init_model('shopdefaultstock')
        self.set_relation(self.mdl_shop, "shop", "shop", "vnum", "name")
        self.set_relation(self.mdl_shop, "model", "model", "vnum", "name")
        self.set_relation(self.mdl_shop, "composition", "material", "vnum", "name")
        self.tbl_shop_default_stock.setModel(self.mdl_shop_default_stock)

        # Issue an update of all the models.
        self.update_models()

    def model_clicked(self, current):
        vnum = self.mdl_model.data(self.mdl_model.index(current.row(), 0))
        self.mdl_model_body_part.setFilter("model = {}".format(vnum))
        self.update_status_bar("Filtering activated: Press ESC to reset filtering.")

    def bodypart_clicked(self, current):
        vnum = self.mdl_bodypart.data(self.mdl_bodypart.index(current.row(), 0))
        self.mdl_bodypart_resources.setFilter("bodyPart = {}".format(vnum))
        self.mdl_bodypart_weapon.setFilter("bodyPart = {}".format(vnum))
        self.update_status_bar("Filtering activated: Press ESC to reset filtering.")

    def building_clicked(self, current):
        vnum = self.mdl_building.data(self.mdl_building.index(current.row(), 0))
        self.mdl_building_ingredient.setFilter("building = {}".format(vnum))
        self.mdl_building_knowledge.setFilter("building = {}".format(vnum))
        self.mdl_building_tool.setFilter("building = {}".format(vnum))
        self.update_status_bar("Filtering activated: Press ESC to reset filtering.")

    def mobile_clicked(self, current):
        vnum = self.mdl_mobile.data(self.mdl_mobile.index(current.row(), 0))
        self.mdl_mobile_spawn.setFilter("mobile = {}".format(vnum))
        self.update_status_bar("Filtering activated: Press ESC to reset filtering.")

    def race_clicked(self, current):
        vnum = self.mdl_race.data(self.mdl_race.index(current.row(), 0))
        self.mdl_race_skill.setFilter("race = {}".format(vnum))
        self.mdl_race_body_part.setFilter("race = {}".format(vnum))
        self.update_status_bar("Filtering activated: Press ESC to reset filtering.")

    def production_clicked(self, current):
        vnum = self.mdl_production.data(self.mdl_production.index(current.row(), 0))
        self.mdl_production_ingredient.setFilter("production = {}".format(vnum))
        self.mdl_production_tool.setFilter("production = {}".format(vnum))
        self.mdl_production_knowledge.setFilter("production = {}".format(vnum))
        self.mdl_production_outcome.setFilter("production = {}".format(vnum))
        self.update_status_bar("Filtering activated: Press ESC to reset filtering.")

    def skill_clicked(self, current):
        vnum = self.mdl_skill.data(self.mdl_skill.index(current.row(), 0))
        self.mdl_skill_prerequisite.setFilter("skill = {}".format(vnum))
        self.mdl_skill_knowledge.setFilter("skill = {}".format(vnum))
        self.mdl_skill_combat_modifier.setFilter("skill = {}".format(vnum))
        self.mdl_skill_ability_modifier.setFilter("skill = {}".format(vnum))
        self.mdl_skill_status_modifier.setFilter("skill = {}".format(vnum))
        self.update_status_bar("Filtering activated: Press ESC to reset filtering.")

    def area_clicked(self, current):
        vnum = self.mdl_area.data(self.mdl_area.index(current.row(), 0))
        self.mdl_area_list.setFilter("area = {}".format(vnum))
        self.mdl_room.setFilter("room.vnum IN (SELECT room AS vnum FROM AreaList WHERE area = {})".format(vnum))
        self.update_status_bar("Filtering activated: Press ESC to reset filtering.")

    def player_clicked(self, current):
        name = self.mdl_player.data(self.mdl_player.index(current.row(), 0))
        self.mdl_player_skill.setFilter("player = '{}'".format(name))
        self.mdl_player_item.setFilter("owner = '{}'".format(name))
        self.mdl_player_variable.setFilter("player = '{}'".format(name))
        self.update_status_bar("Filtering activated: Press ESC to reset filtering.")

    def item_player_mouse_move_event(self, event):
        index = QtCore.QModelIndex(QtCore.QPersistentModelIndex(self.tbl_player_item.indexAt(event.pos())))
        if index != self.item_player_pindex:
            self.item_player_pindex = index
            item_index = self.mdl_player_item.fieldIndex("item")
            vnum = self.mdl_player_item.data(self.mdl_player_item.index(index.row(), item_index))
            query = QtSql.QSqlQuery()
            query.exec("SELECT name FROM Model WHERE vnum = (SELECT model FROM Item WHERE Item.vnum = {})".format(vnum))
            if query.next():
                self.label_player_item_model.setText("{}".format(query.value(0)))

    def item_clicked(self, current):
        name = self.mdl_item.data(self.mdl_item.index(current.row(), 0))
        self.mdl_item_content.setFilter("container = '{}' or item = '{}'".format(name, name))
        self.mdl_item_liquid_content.setFilter("container = '{}'".format(name))
        self.mdl_item_player.setFilter("item = '{}'".format(name))
        self.mdl_item_room.setFilter("item = '{}'".format(name))
        self.update_status_bar("Filtering activated: Press ESC to reset filtering.")

    def shop_clicked(self, current):
        vnum = self.mdl_shop.data(self.mdl_shop.index(current.row(), 0))
        self.mdl_shop_default_stock.setFilter("shop = '{}'".format(vnum))
        self.update_status_bar("Filtering activated: Press ESC to reset filtering.")


if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    styles.apply_dark_theme(app)
    editor = RadMudEditor()
    editor.show()
    sys.exit(app.exec_())
