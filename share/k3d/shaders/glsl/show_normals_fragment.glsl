varying vec3 normal;

void main (void)
{
	gl_FragColor = vec4(normal, 1.0);
}
