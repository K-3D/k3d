// k3d:plugin-class="application"
// k3d:plugin-type="QTUIProgrammableMode"
// k3d:plugin-name="QTUIHelloWorldMode"
// qtui:component-type="mode"

button = new QPushButton("Hello, World!");
button.clicked.connect(function() { print("Hello, World!"); });
proxy = scene.addWidget(button);
proxy.setGeometry(20, 20, 100, 30);

