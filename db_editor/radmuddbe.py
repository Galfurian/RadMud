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

    def close_application():
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

    def setup_tables(self):
        # === MODEL ===========================================================
        mdl = self.mdl_model
        tbl = self.tbl_model
        mdl = QtSql.QSqlRelationalTableModel()
        mdl.setTable('model')
        mdl.setEditStrategy(QtSql.QSqlRelationalTableModel.OnFieldChange)
        mdl.setHeaderData(mdl.fieldIndex("type"), QtCore.Qt.Horizontal, "type")
        mdl.setHeaderData(mdl.fieldIndex("material"), QtCore.Qt.Horizontal, "material")
        mdl.setRelation(mdl.fieldIndex("type"), QtSql.QSqlRelation("ModelType", "id", "name"))
        mdl.setRelation(mdl.fieldIndex("material"), QtSql.QSqlRelation("MaterialType", "id", "name"))
        mdl.select()

        tbl.setModel(mdl)
        tbl.setItemDelegate(QtSql.QSqlRelationalDelegate(tbl))

        # === MATERIAL ========================================================
        mdl = self.mdl_material
        tbl = self.tbl_material
        mdl = QtSql.QSqlRelationalTableModel()
        mdl.setTable('material')
        mdl.setEditStrategy(QtSql.QSqlRelationalTableModel.OnFieldChange)
        mdl.setHeaderData(mdl.fieldIndex("type"), QtCore.Qt.Horizontal, "type")
        mdl.setRelation(mdl.fieldIndex("type"), QtSql.QSqlRelation("MaterialType", "id", "name"))
        mdl.select()

        tbl.setModel(mdl)
        tbl.setItemDelegate(QtSql.QSqlRelationalDelegate(tbl))

        # === CURRENCY ========================================================
        mdl = self.mdl_currency
        tbl = self.tbl_currency
        mdl = QtSql.QSqlRelationalTableModel()
        mdl.setTable('currency')
        mdl.setEditStrategy(QtSql.QSqlRelationalTableModel.OnFieldChange)
        mdl.setHeaderData(mdl.fieldIndex("model"), QtCore.Qt.Horizontal, "model")
        mdl.setHeaderData(mdl.fieldIndex("material"), QtCore.Qt.Horizontal, "material")
        mdl.setRelation(mdl.fieldIndex("model"), QtSql.QSqlRelation("model", "vnum", "name"))
        mdl.setRelation(mdl.fieldIndex("material"), QtSql.QSqlRelation("material", "vnum", "name"))
        mdl.select()

        tbl.setModel(mdl)
        tbl.setItemDelegate(QtSql.QSqlRelationalDelegate(tbl))

        # === BODYPART ========================================================
        mdl = self.mdl_bodypart
        tbl = self.tbl_bodypart
        mdl = QtSql.QSqlRelationalTableModel()
        mdl.setTable('BodyPart')
        mdl.setEditStrategy(QtSql.QSqlRelationalTableModel.OnFieldChange)
        mdl.select()

        tbl.setModel(mdl)
        tbl.setItemDelegate(QtSql.QSqlRelationalDelegate(tbl))
        tbl.clicked.connect(self.bodypart_clicked)
        tbl.keyPressEvent = self.bodypart_key_press_event

        mdl = self.mdl_bodypart_resources
        tbl = self.tbl_bodypart_resources
        mdl = QtSql.QSqlRelationalTableModel()
        mdl.setTable('BodyPartResources')
        mdl.setEditStrategy(QtSql.QSqlRelationalTableModel.OnFieldChange)
        mdl.setHeaderData(mdl.fieldIndex("bodyPart"), QtCore.Qt.Horizontal, "bodyPart")
        mdl.setHeaderData(mdl.fieldIndex("resourceModel"), QtCore.Qt.Horizontal, "resourceModel")
        mdl.setHeaderData(mdl.fieldIndex("material"), QtCore.Qt.Horizontal, "material")
        mdl.setRelation(mdl.fieldIndex("bodyPart"), QtSql.QSqlRelation("BodyPart", "vnum", "name"))
        mdl.setRelation(mdl.fieldIndex("resourceModel"), QtSql.QSqlRelation("Model", "vnum", "name"))
        mdl.setRelation(mdl.fieldIndex("material"), QtSql.QSqlRelation("Material", "vnum", "name"))
        mdl.select()

        tbl.setModel(mdl)
        tbl.setItemDelegate(QtSql.QSqlRelationalDelegate(tbl))

        mdl = self.mdl_bodypart_weapon
        tbl = self.tbl_bodypart_weapon
        mdl = QtSql.QSqlRelationalTableModel()
        mdl.setTable('BodyPartWeapon')
        mdl.setEditStrategy(QtSql.QSqlRelationalTableModel.OnFieldChange)
        mdl.setHeaderData(mdl.fieldIndex("bodyPart"), QtCore.Qt.Horizontal, "bodyPart")
        mdl.setRelation(mdl.fieldIndex("bodyPart"), QtSql.QSqlRelation("BodyPart", "vnum", "name"))
        mdl.select()
        tbl.setModel(mdl)
        tbl.setItemDelegate(QtSql.QSqlRelationalDelegate(tbl))

        # === BUILDING ========================================================
        mdl = self.mdl_building
        tbl = self.tbl_building
        mdl = QtSql.QSqlRelationalTableModel()
        mdl.setTable('building')
        mdl.setEditStrategy(QtSql.QSqlRelationalTableModel.OnFieldChange)
        mdl.setHeaderData(mdl.fieldIndex("model"), QtCore.Qt.Horizontal, "model")
        mdl.setRelation(mdl.fieldIndex("model"), QtSql.QSqlRelation("Model", "vnum", "name"))
        mdl.select()

        tbl.setModel(mdl)
        tbl.setItemDelegate(QtSql.QSqlRelationalDelegate(tbl))
        tbl.clicked.connect(self.building_clicked)
        tbl.keyPressEvent = self.building_key_press_event

        mdl = self.mdl_building_ingredient
        tbl = self.tbl_building_ingredient
        mdl = QtSql.QSqlRelationalTableModel()
        mdl.setTable('BuildingIngredient')
        mdl.setEditStrategy(QtSql.QSqlRelationalTableModel.OnFieldChange)
        mdl.setHeaderData(mdl.fieldIndex("building"), QtCore.Qt.Horizontal, "building")
        mdl.setHeaderData(mdl.fieldIndex("resource"), QtCore.Qt.Horizontal, "resource")
        mdl.setRelation(mdl.fieldIndex("building"), QtSql.QSqlRelation("building", "vnum", "name"))
        mdl.setRelation(mdl.fieldIndex("resource"), QtSql.QSqlRelation("resourcetype", "id", "name"))
        mdl.select()

        tbl.setModel(mdl)
        tbl.setItemDelegate(QtSql.QSqlRelationalDelegate(tbl))

        mdl = self.mdl_building_knowledge
        tbl = self.tbl_building_knowledge
        mdl = QtSql.QSqlRelationalTableModel()
        mdl.setTable('BuildingKnowledge')
        mdl.setEditStrategy(QtSql.QSqlRelationalTableModel.OnFieldChange)
        mdl.setHeaderData(mdl.fieldIndex("building"), QtCore.Qt.Horizontal, "building")
        mdl.setHeaderData(mdl.fieldIndex("knowledge"), QtCore.Qt.Horizontal, "knowledge")
        mdl.setRelation(mdl.fieldIndex("building"), QtSql.QSqlRelation("building", "vnum", "name"))
        mdl.setRelation(mdl.fieldIndex("knowledge"), QtSql.QSqlRelation("knowledge", "id", "name"))
        mdl.select()

        tbl.setModel(mdl)
        tbl.setItemDelegate(QtSql.QSqlRelationalDelegate(tbl))

        mdl = self.mdl_building_tool
        tbl = self.tbl_building_tool
        mdl = QtSql.QSqlRelationalTableModel()
        mdl.setTable('buildingtool')
        mdl.setEditStrategy(QtSql.QSqlRelationalTableModel.OnFieldChange)
        mdl.setHeaderData(mdl.fieldIndex("building"), QtCore.Qt.Horizontal, "building")
        mdl.setHeaderData(mdl.fieldIndex("tool"), QtCore.Qt.Horizontal, "tool")
        mdl.setRelation(mdl.fieldIndex("building"), QtSql.QSqlRelation("building", "vnum", "name"))
        mdl.setRelation(mdl.fieldIndex("tool"), QtSql.QSqlRelation("tooltype", "id", "name"))
        mdl.select()

        tbl.setModel(mdl)
        tbl.setItemDelegate(QtSql.QSqlRelationalDelegate(tbl))

        # === MOBILE ==========================================================
        mdl = self.mdl_mobile
        tbl = self.tbl_mobile
        mdl = QtSql.QSqlRelationalTableModel()
        mdl.setTable('mobilemodel')
        mdl.setEditStrategy(QtSql.QSqlRelationalTableModel.OnFieldChange)
        mdl.setHeaderData(mdl.fieldIndex("race"), QtCore.Qt.Horizontal, "race")
        mdl.setHeaderData(mdl.fieldIndex("faction"), QtCore.Qt.Horizontal, "faction")
        mdl.setRelation(mdl.fieldIndex("race"), QtSql.QSqlRelation("race", "vnum", "name"))
        mdl.setRelation(mdl.fieldIndex("faction"), QtSql.QSqlRelation("faction", "vnum", "name"))
        mdl.select()

        tbl.setModel(mdl)
        tbl.setItemDelegate(QtSql.QSqlRelationalDelegate(tbl))

        mdl = self.mdl_mobile_spawn
        tbl = self.tbl_mobile_spawn
        mdl = QtSql.QSqlRelationalTableModel()
        mdl.setTable('mobilespawn')
        mdl.setEditStrategy(QtSql.QSqlRelationalTableModel.OnFieldChange)
        mdl.setHeaderData(mdl.fieldIndex("mobile"), QtCore.Qt.Horizontal, "mobile")
        mdl.setRelation(mdl.fieldIndex("mobile"), QtSql.QSqlRelation("mobilemodel", "vnum", "propnoun"))
        mdl.select()

        tbl.setModel(mdl)
        tbl.setItemDelegate(QtSql.QSqlRelationalDelegate(tbl))

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


if __name__ == "__main__":
    import sys

    app = QtWidgets.QApplication(sys.argv)
    editor = RadMudEditor()
    sys.exit(app.exec_())
