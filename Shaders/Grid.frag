
// Grid.frag
//�O���b�h�̃t���O�����g�V�F�[�_�[
#version 330 core
out vec4 FragColor;
uniform vec3 uColor;

void main() {
    FragColor = vec4(uColor, 1.0);
}