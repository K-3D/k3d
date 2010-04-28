// k3d:plugin-class="application"
// k3d:plugin-type="QTUIProgrammableMode"
// k3d:plugin-name="QTUIHelloWorldMode"
// qtui:component-type="mode"

button = new QPushButton("Hello, World!");
button.clicked.connect(function() { print("Hello, World!"); });
proxy = scene.addWidget(button);
proxy.setGeometry(20, 20, 50, 30);

rect = new QGraphicsRectItem(100, 100, 200, 200, this);
rect.setBrush(new QBrush(QColor.fromRgbF(0, 1, 0, 0.5)));
rect.setFlag(QGraphicsItem.ItemIsMovable);
scene.addItem(rect);

