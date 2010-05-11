// k3d:plugin-class="application"
// k3d:plugin-type="QTUIProgrammableMode"
// k3d:plugin-name="QTUIConsoleMode"
// qtui:component-type="mode"

function prompt()
{
	widget.console.print_html("<span style='color:green'>&gt;&gt;&gt;</span> ");
}

function execute(command)
{
	widget.command.text = command;
	prompt();
	animation.command.start();
}

// Create states ...
var state = new Object();
state.initial = new QState();
state.running = new QState();
	state.instructions = new QState(state.running);
	state.visible = new QState(state.running);
	state.hidden = new QState(state.running);
state.running.initialState = state.instructions;
state.finished = new QState();

machine = new QStateMachine();
machine.addState(state.initial);
machine.addState(state.running);
machine.addState(state.finished);
machine.initialState = state.initial;

// Create widgets ...
var widget = new Object();
var proxy = new Object();

widget.console = new k3d.console();
widget.console.execute.connect(execute);
widget.console.setWindowOpacity(0.7);
proxy.console = scene.addWidget(widget.console);
prompt();

widget.instructions = new QLabel("Use F2 to toggle the console.");
widget.instructions.alignment = Qt.AlignCenter;
widget.instructions.styleSheet = "QLabel { color: blue;  background: #f5f5dc; }";
widget.instructions.setWindowOpacity(0.7);
proxy.instructions = scene.addWidget(widget.instructions);
proxy.instructions.geometry = new QRect(20, -100, 300, 50);

widget.command = new QLabel();
widget.command.alignment = Qt.AlignCenter;
widget.command.setWindowOpacity(0);
widget.command.styleSheet = "QLabel { color: green; background: #001100; border: 1px solid green; }";
proxy.command = scene.addWidget(widget.command);
proxy.command.geometry = new QRect(20, 400, 600, 50);

// Control widgets based on the current state ...
state.initial.assignProperty(proxy.instructions, "geometry", new QRect(20, -100, 300, 50));
state.initial.assignProperty(proxy.console, "geometry", new QRect(20, -100, 500, 100));

state.instructions.assignProperty(proxy.instructions, "geometry", new QRect(20, 20, 300, 50));

state.visible.assignProperty(proxy.instructions, "geometry", new QRect(20, 1000, 300, 50));
state.visible.assignProperty(proxy.console, "geometry", new QRect(20, 20, 500, 100));
state.visible.entered.connect(widget.console.setFocus);

state.hidden.assignProperty(proxy.console, "geometry", new QRect(20, -100, 500, 100));

state.finished.assignProperty(proxy.instructions, "geometry", new QRect(20, 1000, 300, 50));
state.finished.assignProperty(proxy.console, "geometry", new QRect(20, -100, 500, 100));
state.finished.propertiesAssigned.connect(mode.close);

// Setup transitions between states ...
var transition = new Object();

transition.show_instructions = state.initial.addTransition(state.instructions);

transition.first_show = new QKeyEventTransition(scene.views()[0], QEvent.KeyPress, Qt.Key_F2);
transition.first_show.targetState = state.visible;
state.instructions.addTransition(transition.first_show);

transition.hide = new QKeyEventTransition(scene.views()[0], QEvent.KeyPress, Qt.Key_F2);
transition.hide.targetState = state.hidden;
state.visible.addTransition(transition.hide);

transition.show = new QKeyEventTransition(scene.views()[0], QEvent.KeyPress, Qt.Key_F2);
transition.show.targetState = state.visible;
state.hidden.addTransition(transition.show);

mode.disable_auto_close();
transition.finish = state.running.addTransition(mode, "close_requested()", state.finished);

// Setup animated transitions ...
var animation = new Object();

animation.instructions = new QPropertyAnimation(proxy.instructions, "geometry");
animation.instructions.easingCurve = new QEasingCurve(QEasingCurve.InOutCubic);
animation.instructions.duration = 500;

animation.console = new QPropertyAnimation(proxy.console, "geometry");
animation.console.easingCurve = new QEasingCurve(QEasingCurve.InOutCubic);
animation.console.duration = 500;

animation.command = new QPropertyAnimation(widget.command, "windowOpacity");
animation.command.duration = 1000;
animation.command.startValue = 0;
animation.command.setKeyValueAt(0.5, 1);
animation.command.endValue = 0;
animation.command.easingCurve = new QEasingCurve(QEasingCurve.OutInQuint);

machine.addDefaultAnimation(animation.instructions);
machine.addDefaultAnimation(animation.console);

// Finish setting-up the state machine ...
machine.start();

