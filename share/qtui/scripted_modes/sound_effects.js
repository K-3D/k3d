// k3d:plugin-class="application"
// k3d:plugin-type="QTUIProgrammableMode"
// k3d:plugin-name="QTUISoundEffectsMode"
// qtui:component-type="mode"

function addSoundButton(text, sound, x, y, w, h)
{
	button = new QPushButton(text);
	button.clicked.connect(function()
	{
		print(sound);
		player = Phonon.createPlayer(Phonon.MusicCategory, new MediaSource(sound));
		player.play();
	});
	proxy = scene.addWidget(button);
	proxy.setGeometry(x, y, w, h);
}

addSoundButton("Harp", "/home/tshead/src/k3d/share/qtui/sounds/harp.wav", 20, 20, 120, 30);
addSoundButton("Are You Sure?", "/home/tshead/src/k3d/share/qtui/sounds/areyousure.wav", 20, 50, 120, 30);
addSoundButton("You Cannot!", "/home/tshead/src/k3d/share/qtui/sounds/youcannot.wav", 20, 80, 120, 30);
