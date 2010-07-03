// k3d:plugin-class="application"
// k3d:plugin-type="QTUIProgrammableMode"
// k3d:plugin-name="QTUIStateMachineMode"
// qtui:component-type="mode"

button = new QPushButton("");
button.clicked.connect(function() { print("Voila!"); });
proxy = scene.addWidget(button);
proxy.setGeometry(20, 20, 50, 30);

state1 = new QState();
state1.assignProperty(button, "text", "State 1");
state1.assignProperty(proxy, "geometry", new QRect(20, 20, 50, 30));

state2 = new QState();
state2.assignProperty(button, "text", "State 2");
state2.assignProperty(proxy, "geometry", new QRect(400, 300, 100, 30));

state3 = new QState();
state3.assignProperty(button, "text", "State 3");
state3.assignProperty(proxy, "geometry", new QRect(400, 100, 150, 30));

transition2 = state1.addTransition(button, "clicked()", state2);
transition3 = state2.addTransition(button, "clicked()", state3);
transition1 = state3.addTransition(button, "clicked()", state1);

animation = new QPropertyAnimation(proxy, "geometry");
animation.easingCurve = new QEasingCurve(QEasingCurve.InOutCubic);
animation.duration = 750;
transition1.addAnimation(animation);
transition2.addAnimation(animation);
transition3.addAnimation(animation);

machine = new QStateMachine();
machine.addState(state1);
machine.addState(state2);
machine.addState(state3);
machine.initialState = state1;
machine.start();
