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

        self.setupUi(self)

        self.actionExit.setShortcut("Ctrl+Q")
        self.actionExit.setStatusTip('Close the editor')
        self.actionExit.triggered.connect(self.close_application)

        self.actionOpen.setShortcut("Ctrl+O")
        self.actionOpen.setStatusTip('Open database')
        self.actionOpen.triggered.connect(self.on_click_open_database)

        self.open_database("radmud.db")

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
        self.mdl_model = QtSql.QSqlRelationalTableModel()
        self.mdl_model.setTable('model')
        self.mdl_model.setEditStrategy(QtSql.QSqlRelationalTableModel.OnFieldChange)
        self.mdl_model.setHeaderData(1, QtCore.Qt.Horizontal, "type")
        self.mdl_model.setRelation(1, QtSql.QSqlRelation("ModelType", "id", "name"))
        self.mdl_model.setHeaderData(10, QtCore.Qt.Horizontal, "material")
        self.mdl_model.setRelation(10, QtSql.QSqlRelation("MaterialType", "id", "name"))
        self.mdl_model.select()
        self.tbl_model.setModel(self.mdl_model)
        self.tbl_model.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_model))

        # === MATERIAL ========================================================
        self.mdl_material = QtSql.QSqlRelationalTableModel()
        self.mdl_material.setTable('material')
        self.mdl_material.setEditStrategy(QtSql.QSqlRelationalTableModel.OnFieldChange)
        self.mdl_material.setHeaderData(1, QtCore.Qt.Horizontal, "type")
        self.mdl_material.setRelation(1, QtSql.QSqlRelation("MaterialType", "id", "name"))
        self.mdl_material.select()
        self.tbl_material.setModel(self.mdl_material)
        self.tbl_material.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_material))

        # === CURRENCY ========================================================
        self.mdl_currency = QtSql.QSqlRelationalTableModel()
        self.mdl_currency.setTable('Currency')
        self.mdl_currency.setEditStrategy(QtSql.QSqlRelationalTableModel.OnFieldChange)
        self.mdl_currency.setHeaderData(1, QtCore.Qt.Horizontal, "model")
        self.mdl_currency.setRelation(1, QtSql.QSqlRelation("Model", "vnum", "name"))
        self.mdl_currency.setHeaderData(2, QtCore.Qt.Horizontal, "material")
        self.mdl_currency.setRelation(2, QtSql.QSqlRelation("material", "vnum", "name"))
        self.mdl_currency.select()
        self.tbl_currency.setModel(self.mdl_currency)
        self.tbl_currency.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_currency))

        # === BODYPART ========================================================
        self.mdl_bodypart = QtSql.QSqlRelationalTableModel()
        self.mdl_bodypart.setTable('BodyPart')
        self.mdl_bodypart.setEditStrategy(QtSql.QSqlRelationalTableModel.OnFieldChange)
        self.mdl_bodypart.select()
        self.tbl_bodypart.setModel(self.mdl_bodypart)
        self.tbl_bodypart.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_bodypart))
        self.tbl_bodypart.clicked.connect(self.bodypart_clicked)
        self.tbl_bodypart.keyPressEvent = self.bodypart_key_press_event

        self.mdl_bodypart_resources = QtSql.QSqlRelationalTableModel()
        self.mdl_bodypart_resources.setTable('BodyPartResources')
        self.mdl_bodypart_resources.setEditStrategy(QtSql.QSqlRelationalTableModel.OnFieldChange)
        self.mdl_bodypart_resources.setHeaderData(1, QtCore.Qt.Horizontal, "bodyPart")
        self.mdl_bodypart_resources.setRelation(1, QtSql.QSqlRelation("BodyPart", "vnum", "name"))
        self.mdl_bodypart_resources.setHeaderData(2, QtCore.Qt.Horizontal, "resourceModel")
        self.mdl_bodypart_resources.setRelation(2, QtSql.QSqlRelation("Model", "vnum", "name"))
        self.mdl_bodypart_resources.setHeaderData(3, QtCore.Qt.Horizontal, "material")
        self.mdl_bodypart_resources.setRelation(3, QtSql.QSqlRelation("Material", "vnum", "name"))
        self.mdl_bodypart_resources.select()
        self.tbl_bodypart_resources.setModel(self.mdl_bodypart_resources)
        self.tbl_bodypart_resources.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_bodypart_resources))

        self.mdl_bodypart_weapon = QtSql.QSqlRelationalTableModel()
        self.mdl_bodypart_weapon.setTable('BodyPartWeapon')
        self.mdl_bodypart_weapon.setEditStrategy(QtSql.QSqlRelationalTableModel.OnFieldChange)
        self.mdl_bodypart_weapon.setHeaderData(
            self.mdl_bodypart_weapon.fieldIndex("bodyPart"), QtCore.Qt.Horizontal, "bodyPart")
        self.mdl_bodypart_weapon.setRelation(
            self.mdl_bodypart_weapon.fieldIndex("bodyPart"), QtSql.QSqlRelation("BodyPart", "vnum", "name"))
        self.mdl_bodypart_weapon.select()
        self.tbl_bodypart_weapon.setModel(self.mdl_bodypart_weapon)
        self.tbl_bodypart_weapon.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_bodypart_weapon))

        # === BUILDING ========================================================
        self.mdl_building = QtSql.QSqlRelationalTableModel()
        self.mdl_building.setTable('building')
        self.mdl_building.setEditStrategy(QtSql.QSqlRelationalTableModel.OnFieldChange)
        self.mdl_building.setHeaderData(self.mdl_building.fieldIndex("model"), QtCore.Qt.Horizontal, "model")
        self.mdl_building.setRelation(
            self.mdl_building.fieldIndex("model"), QtSql.QSqlRelation("Model", "vnum", "name"))
        self.mdl_building.select()
        self.tbl_building.setModel(self.mdl_building)
        self.tbl_building.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_building))
        self.tbl_building.clicked.connect(self.building_clicked)
        self.tbl_building.keyPressEvent = self.building_key_press_event

        self.mdl_building_ingredient = QtSql.QSqlRelationalTableModel()
        self.mdl_building_ingredient.setTable('BuildingIngredient')
        self.mdl_building_ingredient.setEditStrategy(QtSql.QSqlRelationalTableModel.OnFieldChange)
        self.mdl_building_ingredient.setHeaderData(
            self.mdl_building_ingredient.fieldIndex("building"), QtCore.Qt.Horizontal, "building")
        self.mdl_building_ingredient.setRelation(
            self.mdl_building_ingredient.fieldIndex("building"), QtSql.QSqlRelation("building", "vnum", "name"))
        self.mdl_building_ingredient.setHeaderData(
            self.mdl_building_ingredient.fieldIndex("resource"), QtCore.Qt.Horizontal, "resource")
        self.mdl_building_ingredient.setRelation(
            self.mdl_building_ingredient.fieldIndex("resource"), QtSql.QSqlRelation("resourcetype", "id", "name"))
        self.mdl_building_ingredient.select()
        self.tbl_building_ingredient.setModel(self.mdl_building_ingredient)
        self.tbl_building_ingredient.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_building_ingredient))

        self.mdl_building_knowledge = QtSql.QSqlRelationalTableModel()
        self.mdl_building_knowledge.setTable('BuildingKnowledge')
        self.mdl_building_knowledge.setEditStrategy(QtSql.QSqlRelationalTableModel.OnFieldChange)
        self.mdl_building_knowledge.setHeaderData(
            self.mdl_building_knowledge.fieldIndex("building"), QtCore.Qt.Horizontal, "building")
        self.mdl_building_knowledge.setRelation(
            self.mdl_building_knowledge.fieldIndex("building"), QtSql.QSqlRelation("building", "vnum", "name"))
        self.mdl_building_knowledge.setHeaderData(
            self.mdl_building_knowledge.fieldIndex("knowledge"), QtCore.Qt.Horizontal, "knowledge")
        self.mdl_building_knowledge.setRelation(
            self.mdl_building_knowledge.fieldIndex("knowledge"), QtSql.QSqlRelation("knowledge", "id", "name"))
        self.mdl_building_knowledge.select()
        self.tbl_building_knowledge.setModel(self.mdl_building_knowledge)
        self.tbl_building_knowledge.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_building_knowledge))

        self.mdl_building_tool = QtSql.QSqlRelationalTableModel()
        self.mdl_building_tool.setTable('buildingtool')
        self.mdl_building_tool.setEditStrategy(QtSql.QSqlRelationalTableModel.OnFieldChange)
        self.mdl_building_tool.setHeaderData(
            self.mdl_building_tool.fieldIndex("building"), QtCore.Qt.Horizontal, "building")
        self.mdl_building_tool.setRelation(
            self.mdl_building_tool.fieldIndex("building"), QtSql.QSqlRelation("building", "vnum", "name"))
        self.mdl_building_tool.setHeaderData(
            self.mdl_building_tool.fieldIndex("tool"), QtCore.Qt.Horizontal, "tool")
        self.mdl_building_tool.setRelation(
            self.mdl_building_tool.fieldIndex("tool"), QtSql.QSqlRelation("tooltype", "id", "name"))
        self.mdl_building_tool.select()
        self.tbl_building_tool.setModel(self.mdl_building_tool)
        self.tbl_building_tool.setItemDelegate(QtSql.QSqlRelationalDelegate(self.tbl_building_tool))

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
