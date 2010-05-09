// k3d:plugin-class="application"
// k3d:plugin-type="QTUIProgrammableMode"
// k3d:plugin-name="QTUIConsoleMode"
// qtui:component-type="mode"

function prompt()
{
	console.print_html("<span style='color:green'>&gt;</span> ");
}

function execute(command)
{
	print(command);
	prompt();
}

button = new QPushButton("Console");
button.shortcut = new QKeySequence("F2");
button_proxy = scene.addWidget(button);
button_proxy.setGeometry(600, 20, 50, 30);

console = new k3d.console();
console.execute.connect(execute);
console.setWindowOpacity(0.7);
console_proxy = scene.addWidget(console);
prompt();

instruction_proxy = scene.addWidget(new QLabel("Use F2 to toggle the console."));
instruction_proxy.widget.alignment = Qt.AlignHCenter;

instructions = new QState();
console_visible = new QState();
console_hidden = new QState();

instructions.assignProperty(instruction_proxy, "geometry", new QRect(20, 20, 500, 100));
instructions.assignProperty(console_proxy, "geometry", new QRect(20, -100, 500, 100));

console_visible.assignProperty(instruction_proxy, "geometry", new QRect(20, 1000, 500, 100));
console_visible.assignProperty(console_proxy, "geometry", new QRect(20, 20, 500, 100));
console_visible.entered.connect(console.setFocus);

console_hidden.assignProperty(console_proxy, "geometry", new QRect(20, -100, 500, 100));

initial_show = instructions.addTransition(button, "clicked()", console_visible);
show = console_hidden.addTransition(button, "clicked()", console_visible);
hide = console_visible.addTransition(button, "clicked()", console_hidden);

instruction_animation = new QPropertyAnimation(instruction_proxy, "geometry");
instruction_animation.easingCurve = new QEasingCurve(QEasingCurve.InOutCubic);
instruction_animation.duration = 500;

console_animation = new QPropertyAnimation(console_proxy, "geometry");
console_animation.easingCurve = new QEasingCurve(QEasingCurve.InOutCubic);
console_animation.duration = 500;

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
