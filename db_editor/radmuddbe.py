# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'radmuddbe.ui'
#
# Created by: PyQt5 UI code generator 5.13.0
#
# WARNING! All changes made in this file will be lost!


from radmuddbe_gui import *

from PyQt5 import QtCore, QtGui, QtWidgets, QtSql


class RadMudEditor(QtWidgets.QMainWindow, Ui_MainWindow):
    def __init__(self):
        super(RadMudEditor, self).__init__()

        self.db_filename = ""
        self.db = None

        # === MODELS ==========================================================
        self.mdl_model = None

        self.mdl_material = None

        self.mdl_currency = None

        self.mdl_bodypart = None
        self.mdl_bodypart_resources = None
        self.mdl_bodypart_weapon = None

        self.mdl_building = None
        self.mdl_building_ingredient = None
        self.mdl_building_knowledge = None
        self.mdl_building_tool = None

        self.mdl_mobile = None
        self.mdl_mobile_spawn = None

        self.mdl_race = None
        self.mdl_race_skill = None
        self.mdl_race_body_part = None

        self.mdl_production = None
        self.mdl_production_ingredient = None
        self.mdl_production_tool = None
        self.mdl_production_knowledge = None
        self.mdl_production_outcome = None

        # Setup UI
        self.setupUi(self)

        self.actionExit.setShortcut("Ctrl+Q")
        self.actionExit.setStatusTip('Close the editor')
        self.actionExit.triggered.connect(self.close_application)

        self.actionOpen.setShortcut("Ctrl+O")
        self.actionOpen.setStatusTip('Open database')
        self.actionOpen.triggered.connect(self.on_click_open_database)

        self.open_database("../system/radmud.db")

        # query = QtSql.QSqlQuery()
        # query.exec("insert into MaterialType values (1, 'Metal')")

        self.show()

    def close_application(self):
        exit(0)

    def open_database(self, filename):
        if filename:
            self.db = QtSql.QSqlDatabase.addDatabase('QSQLITE')
            self.db.setDatabaseName(filename)
            if self.db.open():
                self.db_filename = filename
                self.update_status_bar("Successfully opened database '%s'" % filename)
                self.setup_tables()
            else:
                self.update_status_bar("Cannot connecto to database '%s'" % filename)

    def on_click_open_database(self):
        options = QtWidgets.QFileDialog.Options()
        options |= QtWidgets.QFileDialog.DontUseNativeDialog
        filename, _ = QtWidgets.QFileDialog.getOpenFileName(
            self, "Open database", "",
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
        self.tbl_production.keyPressEvent = self.race_key_press_event

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
        # self.mdl_race_skill.setFilter("production = {}".format(vnum))
        self.update_status_bar("Filtering activated: Click on the table again and press ESC to reset filtering.")

    def production_key_press_event(self, event):
        if event.key() == QtCore.Qt.Key_Escape:
            self.tbl_production.clearSelection()
            # self.mdl_production_skill.setFilter("")
            self.update_status_bar("")


if __name__ == "__main__":
    import sys

    app = QtWidgets.QApplication(sys.argv)
    editor = RadMudEditor()
    sys.exit(app.exec_())
