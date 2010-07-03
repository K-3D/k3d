// k3d:plugin-class="application"
// k3d:plugin-type="QTUIProgrammableMode"
// k3d:plugin-name="QTUISoundEffectsMode"
// qtui:component-type="mode"

function addSoundButton(text, sound)
{
	button = new QPushButton(text);
	button.clicked.connect(function()
	{
		k3d.log.debug(sound);
		player = Phonon.createPlayer(Phonon.MusicCategory, new MediaSource(sound));
		player.play();
	});
	proxy = scene.addWidget(button);
	return proxy;
}

layout = new QGraphicsLinearLayout(Qt.Vertical);
layout.addItem(addSoundButton("Harp", k3d.share_path + "/qtui/sounds/harp.wav"));
layout.addItem(addSoundButton("Are You Sure?", k3d.share_path + "/qtui/sounds/areyousure.wav"));
layout.addItem(addSoundButton("You Cannot!", k3d.share_path + "/qtui/sounds/youcannot.wav"));

form = new QGraphicsWidget();
form.setLayout(layout);
scene.addItem(form);

