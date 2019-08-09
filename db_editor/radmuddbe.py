# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'radmuddbe.ui'
#
# Created by: PyQt5 UI code generator 5.13.0
#
# WARNING! All changes made in this file will be lost!


from radmuddbe_gui import *

from PyQt5 import QtCore, QtGui, QtWidgets, QtSql
from qtmodern import styles


class RadMudEditor(QtWidgets.QMainWindow, Ui_MainWindow):
    def __init__(self):
        super(RadMudEditor, self).__init__()

        # === DATABASE ========================================================
        self.db_filename = ""
        self.db = QtSql.QSqlDatabase.addDatabase('QSQLITE')

        # === MODELS ==========================================================
        self.mdl_model = None
        self.mdl_material = None
        self.mdl_bodypart = None
        self.mdl_bodypart_resources = None
        self.mdl_bodypart_weapon = None
        self.mdl_mobile = None
        self.mdl_mobile_spawn = None
        self.mdl_race = None
        self.mdl_race_skill = None
        self.mdl_race_body_part = None
        self.mdl_skill = None
        self.mdl_skill_prerequisite = None
        self.mdl_skill_knowledge = None
        self.mdl_skill_combat_modifier = None
        self.mdl_skill_ability_modifier = None
        self.mdl_skill_status_modifier = None
        self.mdl_production = None
        self.mdl_production_ingredient = None
        self.mdl_production_tool = None
        self.mdl_production_knowledge = None
        self.mdl_production_outcome = None
        self.mdl_profession = None
        self.mdl_building = None
        self.mdl_building_ingredient = None
        self.mdl_building_knowledge = None
        self.mdl_building_tool = None
        self.mdl_currency = None
        self.mdl_area = None
        self.mdl_area_list = None
        self.mdl_room = None
        self.mdl_player = None
        self.mdl_player_skill = None
        self.mdl_player_item = None
        self.mdl_player_variable = None
        self.mdl_item = None
        self.mdl_item_content = None
        self.mdl_item_liquid_content = None
        self.mdl_item_player = None
        self.mdl_item_room = None

        # === SUPPORT VARIABLES ===============================================
        self.item_player_pindex = QtCore.QModelIndex()

        # === UI SETUP ========================================================
        self.setupUi(self)
        self.actionOpen.triggered.connect(self.on_click_open_database)
        self.actionClose.triggered.connect(self.close_database)
        self.actionSave.triggered.connect(self.save_database)
        self.actionExit.triggered.connect(self.close_application)

        self.tbl_model.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_material.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_bodypart.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_bodypart_resources.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_bodypart_weapon.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_mobile.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_mobile_spawn.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_race.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_race_skill.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_race_body_part.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_skill.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_skill_prerequisite.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_skill_knowledge.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_skill_combat_modifier.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_skill_ability_modifier.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_skill_status_modifier.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_production.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_production_ingredient.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_production_tool.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_production_knowledge.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_production_outcome.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_profession.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_building.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_building_knowledge.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_building_tool.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_building_ingredient.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_currency.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_area.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_area_list.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_room.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_player.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_player_skill.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_player_item.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_player_variable.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_item.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_item_content.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_item_liquid_content.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_item_player.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tbl_item_room.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)

        # === OPEN DATABASE CONNECTION ========================================
        self.open_database("../system/radmud.db")

    def close_application(self):
        self.close_database()
        exit(0)

    def open_database(self, filename):
        if not filename:
            self.update_status_bar("You need to provide a filename!")
            return False
        if self.db.isOpen():
            self.update_status_bar("You are already connected to database '%s'" % filename)
            return False
        self.db.setDatabaseName(filename)
        if not self.db.open():
            self.update_status_bar("Cannot connect to database '%s'" % filename)
            return False
        self.db_filename = filename
        self.setup_tables()
        self.db.transaction()
        self.update_status_bar("Successfully opened database '%s'" % filename)
        return True

    def save_database(self):
        if not self.db.isOpen():
            self.update_status_bar("You are not connected to any database!")
            return False
        if not self.db.commit():
            self.update_status_bar(self.db.lastError())
            return False
        if not self.db.transaction():
            self.update_status_bar(self.db.lastError())
            return False
        self.update_status_bar("Database '%s' saved." % self.db_filename)
        return True

    def close_database(self):
        if not self.db.isOpen():
            self.update_status_bar("The database is already closed!")
            return False
        self.db.close()
        if self.db.isOpen():
            self.update_status_bar("You failed to close the database '%s'!" % self.db_filename)
            return False
        # === UPDATE MODELS ===================================================
        self.mdl_model.select()
        self.mdl_material.select()
        self.mdl_bodypart.select()
        self.mdl_bodypart_resources.select()
        self.mdl_bodypart_weapon.select()
        self.mdl_mobile.select()
        self.mdl_mobile_spawn.select()
        self.mdl_race.select()
        self.mdl_race_skill.select()
        self.mdl_race_body_part.select()
        self.mdl_skill.select()
        self.mdl_skill_prerequisite.select()
        self.mdl_skill_knowledge.select()
        self.mdl_skill_combat_modifier.select()
        self.mdl_skill_ability_modifier.select()
        self.mdl_skill_status_modifier.select()
        self.mdl_production.select()
        self.mdl_production_ingredient.select()
        self.mdl_production_tool.select()
        self.mdl_production_knowledge.select()
        self.mdl_production_outcome.select()
        self.mdl_profession.select()
        self.mdl_building.select()
        self.mdl_building_ingredient.select()
        self.mdl_building_knowledge.select()
        self.mdl_building_tool.select()
        self.mdl_currency.select()
        self.mdl_area.select()
        self.mdl_area_list.select()
        self.mdl_room.select()
        self.mdl_player.select()
        self.mdl_player_skill.select()
        self.mdl_player_item.select()
        self.mdl_player_variable.select()
        self.mdl_item.select()
        self.mdl_item_content.select()
        self.mdl_item_liquid_content.select()
        self.mdl_item_player.select()
        self.mdl_item_room.select()
        self.update_status_bar("Successfully closed database '%s'" % self.db_filename)
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
        model.setEditStrategy(QtSql.QSqlRelationalTableModel.OnFieldChange)
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
        self.mdl_model.select()

        self.tbl_model.setModel(self.mdl_model)
        self.tbl_model.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_model))

        # === MATERIAL ========================================================
        self.mdl_material = self.init_model('material')
        self.set_relation(self.mdl_material, "type", "MaterialType", "id", "name")
        self.mdl_material.select()

        self.tbl_material.setModel(self.mdl_material)
        self.tbl_material.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_material))

        # === CURRENCY ========================================================
        self.mdl_currency = self.init_model('currency')
        self.mdl_currency.setEditStrategy(QtSql.QSqlRelationalTableModel.OnFieldChange)
        self.set_relation(self.mdl_currency, "model", "model", "vnum", "name")
        self.set_relation(self.mdl_currency, "material", "material", "vnum", "name")
        self.mdl_currency.select()

        self.tbl_currency.setModel(self.mdl_currency)
        self.tbl_currency.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_currency))

        # === BODYPART ========================================================
        self.mdl_bodypart = self.init_model('BodyPart')
        self.mdl_bodypart.select()
        self.tbl_bodypart.setModel(self.mdl_bodypart)
        self.tbl_bodypart.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_bodypart))
        self.tbl_bodypart.clicked.connect(self.bodypart_clicked)
        self.tbl_bodypart.keyPressEvent = self.bodypart_key_press_event

        self.mdl_bodypart_resources = self.init_model('BodyPartResources')
        self.set_relation(self.mdl_bodypart_resources, "bodyPart", "bodypart", "vnum", "name")
        self.set_relation(self.mdl_bodypart_resources, "resourceModel", "model", "vnum", "name")
        self.set_relation(self.mdl_bodypart_resources, "material", "material", "vnum", "name")
        self.mdl_bodypart_resources.select()
        self.tbl_bodypart_resources.setModel(self.mdl_bodypart_resources)
        self.tbl_bodypart_resources.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_bodypart_resources))

        self.mdl_bodypart_weapon = self.init_model('BodyPartWeapon')
        self.set_relation(self.mdl_bodypart_weapon, "bodypart", "BodyPart", "vnum", "name")
        self.mdl_bodypart_weapon.select()
        self.tbl_bodypart_weapon.setModel(self.mdl_bodypart_weapon)
        self.tbl_bodypart_weapon.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_bodypart_weapon))

        # === BUILDING ========================================================
        self.mdl_building = self.init_model('building')
        self.set_relation(self.mdl_building, "model", "model", "vnum", "name")
        self.mdl_building.select()
        self.tbl_building.setModel(self.mdl_building)
        self.tbl_building.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_building))
        self.tbl_building.clicked.connect(self.building_clicked)
        self.tbl_building.keyPressEvent = self.building_key_press_event

        self.mdl_building_ingredient = self.init_model('BuildingIngredient')
        self.set_relation(self.mdl_building_ingredient, "building", "building", "vnum", "name")
        self.set_relation(self.mdl_building_ingredient, "resource", "resourcetype", "id", "name")
        self.mdl_building_ingredient.select()
        self.tbl_building_ingredient.setModel(self.mdl_building_ingredient)
        self.tbl_building_ingredient.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_building_ingredient))

        self.mdl_building_knowledge = self.init_model('BuildingKnowledge')
        self.set_relation(self.mdl_building_knowledge, "building", "building", "vnum", "name")
        self.set_relation(self.mdl_building_knowledge, "knowledge", "knowledge", "id", "name")
        self.mdl_building_knowledge.select()
        self.tbl_building_knowledge.setModel(self.mdl_building_knowledge)
        self.tbl_building_knowledge.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_building_knowledge))

        self.mdl_building_tool = self.init_model('buildingtool')
        self.set_relation(self.mdl_building_tool, "building", "building", "vnum", "name")
        self.set_relation(self.mdl_building_tool, "tool", "tooltype", "id", "name")
        self.mdl_building_tool.select()
        self.tbl_building_tool.setModel(self.mdl_building_tool)
        self.tbl_building_tool.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_building_tool))

        # === MOBILE ==========================================================
        self.mdl_mobile = self.init_model('mobilemodel')
        self.set_relation(self.mdl_mobile, "race", "race", "vnum", "name")
        self.set_relation(self.mdl_mobile, "faction", "faction", "vnum", "name")
        self.mdl_mobile.select()
        self.tbl_mobile.setModel(self.mdl_mobile)
        self.tbl_mobile.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_mobile))
        self.tbl_mobile.clicked.connect(self.mobile_clicked)
        self.tbl_mobile.keyPressEvent = self.mobile_key_press_event

        self.mdl_mobile_spawn = self.init_model('mobilespawn')
        self.set_relation(self.mdl_mobile_spawn, "mobile", "mobilemodel", "vnum", "propnoun")
        self.mdl_mobile_spawn.select()
        self.tbl_mobile_spawn.setModel(self.mdl_mobile_spawn)
        self.tbl_mobile_spawn.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_mobile_spawn))

        # === RACE ============================================================
        self.mdl_race = self.init_model('race')
        self.mdl_race.select()
        self.tbl_race.setModel(self.mdl_race)
        self.tbl_race.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_race))
        self.tbl_race.clicked.connect(self.race_clicked)
        self.tbl_race.keyPressEvent = self.race_key_press_event

        self.mdl_race_skill = self.init_model('racebaseskill')
        self.set_relation(self.mdl_race_skill, "race", "race", "vnum", "name")
        self.set_relation(self.mdl_race_skill, "skill", "skill", "vnum", "name")
        self.mdl_race_skill.select()
        self.tbl_race_skill.setModel(self.mdl_race_skill)
        self.tbl_race_skill.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_race_skill))

        self.mdl_race_body_part = self.init_model('racebodypart')
        self.set_relation(self.mdl_race_body_part, "race", "race", "vnum", "name")
        self.set_relation(self.mdl_race_body_part, "bodypart", "bodypart", "vnum", "name")
        self.mdl_race_body_part.select()
        self.tbl_race_body_part.setModel(self.mdl_race_body_part)
        self.tbl_race_body_part.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_race_body_part))

        # === PRODUCTION ======================================================
        self.mdl_production = self.init_model('production')
        self.set_relation(self.mdl_production, "profession", "profession", "vnum", "command")
        self.set_relation(self.mdl_production, "workbench", "tooltype", "id", "name")
        self.mdl_production.select()
        self.tbl_production.setModel(self.mdl_production)
        self.tbl_production.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_production))
        self.tbl_production.clicked.connect(self.production_clicked)
        self.tbl_production.keyPressEvent = self.production_key_press_event

        self.mdl_production_ingredient = self.init_model('productioningredient')
        self.set_relation(self.mdl_production_ingredient, "production", "production", "vnum", "name")
        self.set_relation(self.mdl_production_ingredient, "ingredient", "resourcetype", "id", "name")
        self.mdl_production_ingredient.select()
        self.tbl_production_ingredient.setModel(self.mdl_production_ingredient)
        self.tbl_production_ingredient.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_production_ingredient))

        self.mdl_production_tool = self.init_model('productiontool')
        self.set_relation(self.mdl_production_tool, "production", "production", "vnum", "name")
        self.set_relation(self.mdl_production_tool, "tool", "tooltype", "id", "name")
        self.mdl_production_tool.select()
        self.tbl_production_tool.setModel(self.mdl_production_tool)
        self.tbl_production_tool.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_production_tool))

        self.mdl_production_knowledge = self.init_model('productionknowledge')
        self.set_relation(self.mdl_production_knowledge, "production", "production", "vnum", "name")
        self.set_relation(self.mdl_production_knowledge, "knowledge", "knowledge", "id", "name")
        self.mdl_production_knowledge.select()
        self.tbl_production_knowledge.setModel(self.mdl_production_knowledge)
        self.tbl_production_knowledge.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_production_knowledge))

        self.mdl_production_outcome = self.init_model('productionoutcome')
        self.set_relation(self.mdl_production_outcome, "production", "production", "vnum", "name")
        self.set_relation(self.mdl_production_outcome, "outcome", "model", "vnum", "name")
        self.mdl_production_outcome.select()
        self.tbl_production_outcome.setModel(self.mdl_production_outcome)
        self.tbl_production_outcome.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_production_outcome))

        # === PROFESSION ======================================================
        self.mdl_profession = self.init_model('profession')
        self.mdl_profession.select()
        self.tbl_profession.setModel(self.mdl_profession)
        self.tbl_profession.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_profession))

        # === SKILL ===========================================================
        # self.tbl_skill.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        # setSectionResizeMode(QHeaderView::Stretch);
        self.mdl_skill = self.init_model('skill')
        self.set_relation(self.mdl_skill, "ability", "abilitytype", "id", "name")
        self.mdl_skill.select()
        self.tbl_skill.setModel(self.mdl_skill)
        self.tbl_skill.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_skill))
        self.tbl_skill.clicked.connect(self.skill_clicked)
        self.tbl_skill.keyPressEvent = self.skill_key_press_event

        self.mdl_skill_prerequisite = self.init_model('skillprerequisite')
        self.set_relation(self.mdl_skill_prerequisite, "skill", "skill", "vnum", "name")
        self.set_relation(self.mdl_skill_prerequisite, "requiredskill", "skill", "vnum", "name")
        self.mdl_skill_prerequisite.select()
        self.tbl_skill_prerequisite.setModel(self.mdl_skill_prerequisite)
        self.tbl_skill_prerequisite.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_skill_prerequisite))

        self.mdl_skill_knowledge = self.init_model('skillknowledge')
        self.set_relation(self.mdl_skill_knowledge, "skill", "skill", "vnum", "name")
        self.set_relation(self.mdl_skill_knowledge, "knowledge", "knowledge", "id", "name")
        self.mdl_skill_knowledge.select()
        self.tbl_skill_knowledge.setModel(self.mdl_skill_knowledge)
        self.tbl_skill_knowledge.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_skill_knowledge))

        self.mdl_skill_combat_modifier = self.init_model('skillcombatmodifier')
        self.set_relation(self.mdl_skill_combat_modifier, "skill", "skill", "vnum", "name")
        self.set_relation(self.mdl_skill_combat_modifier, "modifier", "combatmodifier", "id", "name")
        self.mdl_skill_combat_modifier.select()
        self.tbl_skill_combat_modifier.setModel(self.mdl_skill_combat_modifier)
        self.tbl_skill_combat_modifier.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_skill_combat_modifier))

        self.mdl_skill_ability_modifier = self.init_model('skillabilitymodifier')
        self.set_relation(self.mdl_skill_ability_modifier, "skill", "skill", "vnum", "name")
        self.set_relation(self.mdl_skill_ability_modifier, "ability", "abilitytype", "id", "name")
        self.mdl_skill_ability_modifier.select()
        self.tbl_skill_ability_modifier.setModel(self.mdl_skill_ability_modifier)
        self.tbl_skill_ability_modifier.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_skill_ability_modifier))

        self.mdl_skill_status_modifier = self.init_model('skillstatusmodifier')
        self.set_relation(self.mdl_skill_status_modifier, "skill", "skill", "vnum", "name")
        self.set_relation(self.mdl_skill_status_modifier, "status", "statusmodifier", "id", "name")
        self.mdl_skill_status_modifier.select()
        self.tbl_skill_status_modifier.setModel(self.mdl_skill_status_modifier)
        self.tbl_skill_status_modifier.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_skill_status_modifier))

        # === AREA ============================================================
        self.mdl_area = self.init_model('area')
        self.mdl_area.select()
        self.tbl_area.setModel(self.mdl_area)
        self.tbl_area.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_area))
        self.tbl_area.clicked.connect(self.area_clicked)
        self.tbl_area.keyPressEvent = self.area_key_press_event

        self.mdl_area_list = self.init_model('arealist')
        self.set_relation(self.mdl_area_list, "area", "area", "vnum", "name")
        self.mdl_area_list.select()
        self.tbl_area_list.setModel(self.mdl_area_list)
        self.tbl_area_list.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_area_list))

        self.mdl_room = self.init_model('room')
        self.set_relation(self.mdl_room, "terrain", "terrain", "vnum", "name")
        self.mdl_room.select()
        self.tbl_room.setModel(self.mdl_room)
        self.tbl_room.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_room))

        # === PLAYER ==========================================================
        self.mdl_player = self.init_model('player')
        self.mdl_player.select()
        self.tbl_player.setModel(self.mdl_player)
        self.tbl_player.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_player))
        self.tbl_player.clicked.connect(self.player_clicked)
        self.tbl_player.keyPressEvent = self.player_key_press_event

        self.mdl_player_skill = self.init_model('playerskill')
        self.set_relation(self.mdl_player_skill, "skill", "skill", "vnum", "name")
        self.mdl_player_skill.select()
        self.tbl_player_skill.setModel(self.mdl_player_skill)
        self.tbl_player_skill.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_player_skill))

        self.mdl_player_item = self.init_model('itemplayer')
        self.set_relation(self.mdl_player_item, "position", "bodypart", "vnum", "name")
        self.mdl_player_item.select()
        self.tbl_player_item.setModel(self.mdl_player_item)
        self.tbl_player_item.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_player_item))
        # self.tbl_player_item.clicked.connect(self.player_item_clicked)
        self.tbl_player_item.setMouseTracking(True)
        self.tbl_player_item.mouseMoveEvent = self.item_player_mouse_move_event

        self.mdl_player_variable = self.init_model('playervariable')
        self.mdl_player_variable.select()
        self.tbl_player_variable.setModel(self.mdl_player_variable)
        self.tbl_player_variable.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_player_variable))

        # === ITEM ============================================================
        self.mdl_item = self.init_model('item')
        self.set_relation(self.mdl_item, "model", "model", "vnum", "name")
        self.mdl_item.select()
        self.tbl_item.setModel(self.mdl_item)
        self.tbl_item.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_item))
        self.tbl_item.clicked.connect(self.item_clicked)
        self.tbl_item.keyPressEvent = self.item_key_press_event

        self.mdl_item_content = self.init_model('itemcontent')
        self.mdl_item_content.select()
        self.tbl_item_content.setModel(self.mdl_item_content)
        self.tbl_item_content.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_item_content))

        self.mdl_item_liquid_content = self.init_model('itemliquidcontent')
        self.set_relation(self.mdl_item_liquid_content, "content", "liquid", "vnum", "name")
        self.mdl_item_liquid_content.select()
        self.tbl_item_liquid_content.setModel(self.mdl_item_liquid_content)
        self.tbl_item_liquid_content.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_item_liquid_content))

        self.mdl_item_player = self.init_model('itemplayer')
        self.set_relation(self.mdl_item_player, "position", "bodypart", "vnum", "name")
        self.mdl_item_player.select()
        self.tbl_item_player.setModel(self.mdl_item_player)
        self.tbl_item_player.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_item_player))

        self.mdl_item_room = self.init_model('itemroom')
        self.mdl_item_room.select()
        self.tbl_item_room.setModel(self.mdl_item_room)
        self.tbl_item_room.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_item_content))

    def handleItemEntered(self, item):
        print("casso")

    def bodypart_clicked(self, current):
        vnum = self.mdl_bodypart.data(self.mdl_bodypart.index(current.row(), 0))
        self.mdl_bodypart_resources.setFilter("BodyPartResources.bodyPart = {}".format(vnum))
        self.mdl_bodypart_resources.select()
        self.mdl_bodypart_weapon.setFilter("BodyPartWeapon.bodyPart = {}".format(vnum))
        self.mdl_bodypart_weapon.select()
        self.update_status_bar("Filtering activated: Click on the table again and press ESC to reset filtering.")

    def bodypart_key_press_event(self, event):
        if event.key() == QtCore.Qt.Key_Escape:
            self.tbl_bodypart.clearSelection()
            self.mdl_bodypart_resources.setFilter("")
            self.mdl_bodypart_resources.select()
            self.mdl_bodypart_weapon.setFilter("")
            self.mdl_bodypart_weapon.select()
            self.update_status_bar("")

    def building_clicked(self, current):
        vnum = self.mdl_building.data(self.mdl_building.index(current.row(), 0))

        self.mdl_building_ingredient.setFilter("building = {}".format(vnum))
        self.mdl_building_ingredient.select()
        self.mdl_building_knowledge.setFilter("building = {}".format(vnum))
        self.mdl_building_knowledge.select()
        self.mdl_building_tool.setFilter("building = {}".format(vnum))
        self.mdl_building_tool.select()

        self.update_status_bar("Filtering activated: Click on the table again and press ESC to reset filtering.")

    def building_key_press_event(self, event):
        if event.key() == QtCore.Qt.Key_Escape:
            self.tbl_building.clearSelection()
            self.mdl_building_ingredient.setFilter("")
            self.mdl_building_ingredient.select()
            self.mdl_building_knowledge.setFilter("")
            self.mdl_building_knowledge.select()
            self.mdl_building_tool.setFilter("")
            self.mdl_building_tool.select()
            self.update_status_bar("")

    def mobile_clicked(self, current):
        vnum = self.mdl_mobile.data(self.mdl_mobile.index(current.row(), 0))

        self.mdl_mobile_spawn.setFilter("mobile = {}".format(vnum))
        self.mdl_mobile_spawn.select()

        self.update_status_bar("Filtering activated: Click on the table again and press ESC to reset filtering.")

    def mobile_key_press_event(self, event):
        if event.key() == QtCore.Qt.Key_Escape:
            self.tbl_mobile.clearSelection()
            self.mdl_mobile_spawn.setFilter("")
            self.mdl_mobile_spawn.select()
            self.update_status_bar("")

    def race_clicked(self, current):
        vnum = self.mdl_race.data(self.mdl_race.index(current.row(), 0))
        self.mdl_race_skill.setFilter("race = {}".format(vnum))
        self.mdl_race_skill.select()
        self.mdl_race_body_part.setFilter("race = {}".format(vnum))
        self.mdl_race_body_part.select()
        self.update_status_bar("Filtering activated: Click on the table again and press ESC to reset filtering.")

    def race_key_press_event(self, event):
        if event.key() == QtCore.Qt.Key_Escape:
            self.tbl_race.clearSelection()
            self.mdl_race_skill.setFilter("")
            self.mdl_race_body_part.setFilter("")
            self.update_status_bar("")

    def production_clicked(self, current):
        vnum = self.mdl_production.data(self.mdl_production.index(current.row(), 0))
        self.mdl_production_ingredient.setFilter("production = {}".format(vnum))
        self.mdl_production_tool.setFilter("production = {}".format(vnum))
        self.mdl_production_knowledge.setFilter("production = {}".format(vnum))
        self.mdl_production_outcome.setFilter("production = {}".format(vnum))
        self.update_status_bar("Filtering activated: Click on the table again and press ESC to reset filtering.")

    def production_key_press_event(self, event):
        if event.key() == QtCore.Qt.Key_Escape:
            self.tbl_production.clearSelection()
            self.mdl_production_ingredient.setFilter("")
            self.mdl_production_tool.setFilter("")
            self.mdl_production_knowledge.setFilter("")
            self.mdl_production_outcome.setFilter("")
            self.update_status_bar("")

    def skill_clicked(self, current):
        vnum = self.mdl_skill.data(self.mdl_skill.index(current.row(), 0))
        self.mdl_skill_prerequisite.setFilter("skill = {}".format(vnum))
        self.mdl_skill_knowledge.setFilter("skill = {}".format(vnum))
        self.mdl_skill_combat_modifier.setFilter("skill = {}".format(vnum))
        self.mdl_skill_ability_modifier.setFilter("skill = {}".format(vnum))
        self.mdl_skill_status_modifier.setFilter("skill = {}".format(vnum))
        self.update_status_bar("Filtering activated: Click on the table again and press ESC to reset filtering.")

    def skill_key_press_event(self, event):
        if event.key() == QtCore.Qt.Key_Escape:
            self.tbl_skill.clearSelection()
            self.mdl_skill_prerequisite.setFilter("")
            self.mdl_skill_knowledge.setFilter("")
            self.mdl_skill_combat_modifier.setFilter("")
            self.mdl_skill_ability_modifier.setFilter("")
            self.mdl_skill_status_modifier.setFilter("")
            self.update_status_bar("")

    def area_clicked(self, current):
        vnum = self.mdl_area.data(self.mdl_area.index(current.row(), 0))
        self.mdl_area_list.setFilter("area = {}".format(vnum))
        self.mdl_room.setFilter("room.vnum IN (SELECT room AS vnum FROM AreaList WHERE area = {})".format(vnum))
        self.update_status_bar("Filtering activated: Click on the table again and press ESC to reset filtering.")

    def area_key_press_event(self, event):
        if event.key() == QtCore.Qt.Key_Escape:
            self.tbl_area.clearSelection()
            self.mdl_area_list.setFilter("")
            self.mdl_room.setFilter("")
            self.update_status_bar("")

    def player_clicked(self, current):
        name = self.mdl_player.data(self.mdl_player.index(current.row(), 0))
        self.mdl_player_skill.setFilter("player = '{}'".format(name))
        self.mdl_player_item.setFilter("owner = '{}'".format(name))
        self.mdl_player_variable.setFilter("player = '{}'".format(name))
        self.update_status_bar("Filtering activated: Click on the table again and press ESC to reset filtering.")

    def player_key_press_event(self, event):
        if event.key() == QtCore.Qt.Key_Escape:
            self.tbl_player.clearSelection()
            self.mdl_player_skill.setFilter("")
            self.mdl_player_item.setFilter("")
            self.mdl_player_variable.setFilter("")
            self.update_status_bar("")

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
        self.update_status_bar("Filtering activated: Click on the table again and press ESC to reset filtering.")

    def item_key_press_event(self, event):
        if event.key() == QtCore.Qt.Key_Escape:
            self.tbl_item.clearSelection()
            self.mdl_item_content.setFilter("")
            self.mdl_item_liquid_content.setFilter("")
            self.mdl_item_player.setFilter("")
            self.mdl_item_room.setFilter("")
            self.update_status_bar("")


if __name__ == "__main__":
    import sys

    app = QtWidgets.QApplication(sys.argv)
    styles.apply_dark_theme(app)
    editor = RadMudEditor()
    editor.show()
    sys.exit(app.exec_())
