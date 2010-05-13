// k3d:plugin-class="application"
// k3d:plugin-type="QTUIProgrammableMode"
// k3d:plugin-name="QTUIBrowserMode"
// qtui:component-type="mode"

// Create states ...
var state = new Object();
state.initial = new QState();
state.running = new QState();
state.finished = new QState();

state.machine = new QStateMachine();
state.machine.addState(state.initial);
state.machine.addState(state.running);
state.machine.addState(state.finished);
state.machine.initialState = state.initial;

// Create widgets
var widget = new Object();
var proxy = new Object();

widget.browser = new QWebView();
widget.browser.setWindowOpacity(0.7);
proxy.browser = scene.addWidget(widget.browser);
//proxy.browser.setGeometry(50, 50, 600, 400);

/*
back_button = new QPushButton("Back");
back_button.clicked.connect(function() { browser.back(); });
proxy = scene.addWidget(back_button);
proxy.setGeometry(50, 20, 100, 20);

forward_button = new QPushButton("Forward");
forward_button.clicked.connect(function() { browser.forward(); });
proxy = scene.addWidget(forward_button);
proxy.setGeometry(170, 20, 100, 20);
*/

// Control widgets based on the current state ...
state.initial.assignProperty(proxy.browser, "geometry", new QRect(-600, 50, 600, 400));

state.running.assignProperty(proxy.browser, "geometry", new QRect(50, 50, 600, 400));
state.running.propertiesAssigned.connect(function() { widget.browser.load(new QUrl("http://www.k-3d.org")); });

state.finished.assignProperty(proxy.browser, "geometry", new QRect(-600, 50, 600, 400));
state.finished.propertiesAssigned.connect(function() { scene.set_active_mode("QTUIConsoleMode"); });

// Setup transitions between states ...
var transition = new Object();

transition.start = state.initial.addTransition(state.running);

transition.finish = new QKeyEventTransition(scene.views()[0], QEvent.KeyPress, Qt.Key_C);
transition.finish.targetState = state.finished;
state.running.addTransition(transition.finish);

// Setup animated transitions ...
var animation = new Object();

animation.browser = new QPropertyAnimation(proxy.browser, "geometry");
animation.browser.easingCurve = new QEasingCurve(QEasingCurve.InOutCubic);
animation.browser.duration = 500;

transition.start.addAnimation(animation.browser);
transition.finish.addAnimation(animation.browser);

// Finish setting-up the state machine ...
state.machine.start();

