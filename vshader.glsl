uniform mat4 mvp_matrix;
uniform float colorOffset;
uniform vec4 selectColor;
uniform float step;

attribute vec4 a_position;

varying vec3 color;

void main()
{
  gl_Position = mvp_matrix * a_position;
  if ( a_position.z < step ) {
    color = vec3(a_position.z - colorOffset, a_position.z - colorOffset, a_position.z - colorOffset);
  }
  else {
    color = selectColor.xyz;
  }
}
