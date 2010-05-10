// k3d:plugin-class="application"
// k3d:plugin-type="QTUIProgrammableMode"
// k3d:plugin-name="QTUIConsoleMode"
// qtui:component-type="mode"

function prompt()
{
	console.print_html("<span style='color:green'>&gt;&gt;&gt;</span> ");
}

function execute(command)
{
	command_label.text = command;
	prompt();
	command_animation.start();
}

console = new k3d.console();
console.execute.connect(execute);
console.setWindowOpacity(0.7);
console_proxy = scene.addWidget(console);
prompt();

instruction_label = new QLabel("Use F2 to toggle the console.");
instruction_label.alignment = Qt.AlignCenter;
instruction_label.styleSheet = "QLabel { color: blue;  background: #f5f5dc; }";
instruction_label.setWindowOpacity(0.7);
instruction_proxy = scene.addWidget(instruction_label);

command_label = new QLabel();
command_label.alignment = Qt.AlignCenter;
command_label.setWindowOpacity(0);
command_label.styleSheet = "QLabel { color: green; background: #001100; border: 1px solid green; }";
command_proxy = scene.addWidget(command_label);
command_proxy.geometry = new QRect(20, 400, 600, 50);

instructions = new QState();
instructions.assignProperty(instruction_proxy, "geometry", new QRect(20, 20, 300, 50));
instructions.assignProperty(console_proxy, "geometry", new QRect(20, -100, 500, 100));

console_visible = new QState();
console_visible.assignProperty(instruction_proxy, "geometry", new QRect(20, 1000, 300, 50));
console_visible.assignProperty(console_proxy, "geometry", new QRect(20, 20, 500, 100));
console_visible.entered.connect(console.setFocus);

console_hidden = new QState();
console_hidden.assignProperty(console_proxy, "geometry", new QRect(20, -100, 500, 100));

initial_show = new QKeyEventTransition(scene.views()[0], QEvent.KeyPress, Qt.Key_F2);
initial_show.targetState = console_visible;

show = new QKeyEventTransition(scene.views()[0], QEvent.KeyPress, Qt.Key_F2);
show.targetState = console_visible;

hide = new QKeyEventTransition(scene.views()[0], QEvent.KeyPress, Qt.Key_F2);
hide.targetState = console_hidden;

instructions.addTransition(initial_show);
console_hidden.addTransition(show);
console_visible.addTransition(hide);

instruction_animation = new QPropertyAnimation(instruction_proxy, "geometry");
instruction_animation.easingCurve = new QEasingCurve(QEasingCurve.InOutCubic);
instruction_animation.duration = 500;

console_animation = new QPropertyAnimation(console_proxy, "geometry");
console_animation.easingCurve = new QEasingCurve(QEasingCurve.InOutCubic);
console_animation.duration = 500;

command_animation = new QPropertyAnimation(command_label, "windowOpacity");
command_animation.duration = 800;
command_animation.startValue = 0;
command_animation.setKeyValueAt(0.5, 1);
command_animation.endValue = 0;
command_animation.easingCurve = new QEasingCurve(QEasingCurve.OutInQuint);

initial_show.addAnimation(instruction_animation);
initial_show.addAnimation(console_animation);
show.addAnimation(console_animation);
hide.addAnimation(console_animation);

machine = new QStateMachine();
machine.addState(instructions);
machine.addState(console_hidden);
machine.addState(console_visible);
machine.initialState = instructions;
machine.start();
