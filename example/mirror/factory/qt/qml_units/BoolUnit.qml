import QtQuick 2.2
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import "qrc:///qml_units"

Checkbox {
    property variant model: null
	text: model.name
}
