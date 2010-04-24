button = new QPushButton("Hello, World!");
button.clicked.connect(function() { print("Hello, World!"); });
proxy = scene.addWidget(button);
proxy.setGeometry(20, 20, 100, 30);

