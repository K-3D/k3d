// k3d:plugin-class="application"
// k3d:plugin-type="QTUIProgrammableMode"
// k3d:plugin-name="QTUIBrowserMode"
// qtui:component-type="mode"

browser = new QWebView();
browser.setWindowOpacity(0.7);
browser.load(new QUrl("http://www.k-3d.org"));
proxy = scene.addWidget(browser);
proxy.setGeometry(50, 50, 600, 400);

back_button = new QPushButton("Back");
back_button.clicked.connect(function() { browser.back(); });
proxy = scene.addWidget(back_button);
proxy.setGeometry(50, 20, 100, 20);

forward_button = new QPushButton("Forward");
forward_button.clicked.connect(function() { browser.forward(); });
proxy = scene.addWidget(forward_button);
proxy.setGeometry(170, 20, 100, 20);

