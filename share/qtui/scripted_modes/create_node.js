// k3d:plugin-class="application"
// k3d:plugin-type="QTUIProgrammableMode"
// k3d:plugin-name="QTUICreateMode"
// qtui:component-type="mode"

edit = new QLineEdit();
edit.setWindowOpacity(0.6);
edit.returnPressed.connect(
	function()
	{
		factory_name = edit.text;
		node_name = factory_name;
		k3d.plugin.create(factory_name, document, node_name);
	});

proxy = scene.addWidget(edit);
proxy.setGeometry(20, 20, 250, 30);

plugin_names = new Array();
plugins = k3d.plugin.factory.lookup();
for(var i = 0; i != plugins.length; ++i)
{
  plugin_names.push(plugins[i].name);
}

completer = new QCompleter(plugin_names.sort(), edit);
completer.completionMode = QCompleter.PopupCompletion;
completer.caseSensitivity = Qt.CaseInsensitive;
completer.modelSorting = QCompleter.CaseSensitivelySortedModel;
edit.setCompleter(completer);

