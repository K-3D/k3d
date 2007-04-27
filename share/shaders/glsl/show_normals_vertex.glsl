varying vec3 normal;

void main(void)
{
	normal = gl_Normal;
	gl_Position = ftransform();
}
