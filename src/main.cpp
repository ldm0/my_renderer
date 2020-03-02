#include"my_renderer.h"
#include"my_obj_loader/my_obj_loader.h"
#include<Windows.h>
#include<tchar.h>
#include<time.h>
#include<math.h>
#include<stdio.h>	// for print debug info
#include<stdlib.h>	// for malloc space for storing model mesh

#define renderer Renderer::get()

bool key_down[256] = {false};
bool window_exit = false;
bool mouse_drag = false;
f3 yaw_pitch_roll_before = {0};
WORD mouse_drag_position_x = 0;
WORD mouse_drag_position_y = 0;
WORD mouse_position_x = 0;
WORD mouse_position_y = 0;

const char *mesh_file_path = "./assets/monkey.obj";

LRESULT CALLBACK event_process(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case WM_DESTROY:
		window_exit = true;
		break;
	case WM_LBUTTONDOWN:
		key_down[VK_LBUTTON] = true;
		break;
	case WM_LBUTTONUP:
		key_down[VK_LBUTTON] = false;
		break;
	case WM_KEYDOWN:
		key_down[wparam] = true;
		break;
	case WM_KEYUP:
		key_down[wparam] = false;
		break;
	case WM_MOUSEMOVE:
		mouse_position_x = LOWORD(lparam);
		mouse_position_y = HIWORD(lparam);
        break;
	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}

void event_dispatch(void)
{
	MSG msg;
	while (PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE)) {
		if (!GetMessage(&msg, 0, 0, 0))
			break;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void camera_reset(void)
{
	renderer.camera_position = {0.f, 0.f, -4.f, 1.f};
	renderer.yaw_pitch_roll = {0.f, 0.f, 0.f};
}

void camera_control(void)
{
#define move_speed (0.03f)
#define rotate_speed (0.01f)
	f4 camera_direction = {0.f, 0.f, 1.f, 0.f};
	camera_direction = rotate(&camera_direction, &(renderer.yaw_pitch_roll));
	f4 up_direction = {0.f, 1.f, 0.f, 0.f};
	f4 right_direction = cross_product(&camera_direction, &up_direction);
	right_direction.x *= -1.f;
	right_direction.y *= -1.f;
	right_direction.z *= -1.f;

	static bool log = false;
	if (key_down['L']) {
		if (!log) {
			right_direction = normalize_vec4(&right_direction);
			printf("Camera position\t\t: x:%.2f y:%.2f z:%.2f\n", renderer.camera_position.x, renderer.camera_position.y, renderer.camera_position.z);
			printf("Camera direction\t: x:%.2f y:%.2f z:%.2f\n\n", camera_direction.x, camera_direction.y, camera_direction.z);
			log = true;
		}
	} else {
		log = false;
	}
	if (key_down[VK_SPACE])
		renderer.camera_position.y += move_speed;
	if (key_down[VK_CONTROL])
		renderer.camera_position.y -= move_speed;
	if (key_down[VK_LEFT])
		renderer.yaw_pitch_roll.x += rotate_speed;
	if (key_down[VK_RIGHT])
		renderer.yaw_pitch_roll.x -= rotate_speed;
	// plus and minus are reversed is because pitch means angle from y to z
	if (key_down[VK_UP])
		renderer.yaw_pitch_roll.y -= rotate_speed;
	if (key_down[VK_DOWN])
		renderer.yaw_pitch_roll.y += rotate_speed;
	if (key_down['Q'])
		renderer.yaw_pitch_roll.z += rotate_speed;
	if (key_down['E'])
		renderer.yaw_pitch_roll.z -= rotate_speed;
	if (key_down['W'])
		renderer.camera_position = translate(&(renderer.camera_position), &camera_direction, move_speed);
	if (key_down['S'])
		renderer.camera_position = translate(&(renderer.camera_position), &camera_direction, -move_speed);
	if (key_down['D'])
		renderer.camera_position = translate(&(renderer.camera_position), &right_direction, move_speed);
	if (key_down['A'])
		renderer.camera_position = translate(&(renderer.camera_position), &right_direction, -move_speed);
	// for debug
	// printf("Position: %f %f %f\n", renderer.camera_position.x, renderer.camera_position.y, renderer.camera_position.z);
	if (key_down['R'])
		camera_reset();
	static bool mode_change = false;
	if (key_down['M']) {
		if (!mode_change) {
			renderer.draw_mode = (Renderer::DRAW_MODE)((renderer.draw_mode + 1) % 3);
			mode_change = true;
		}
	} else {
		mode_change = false;
	}
	if (key_down[VK_LBUTTON]) {
		if (!mouse_drag) {
			yaw_pitch_roll_before = renderer.yaw_pitch_roll;
			mouse_drag_position_x = mouse_position_x;
			mouse_drag_position_y = mouse_position_y;
			mouse_drag = true;
		} else {
			renderer.yaw_pitch_roll.x = yaw_pitch_roll_before.x - (mouse_position_x - mouse_drag_position_x) * rotate_speed / 10.f;
			renderer.yaw_pitch_roll.y = yaw_pitch_roll_before.y + (mouse_position_y - mouse_drag_position_y) * rotate_speed / 10.f;
		}
	} else {
		mouse_drag = false;
	}
#undef move_speed
#undef rotate_speed
}

int main()
{
	my_obj_set_allocator(malloc, free);
	renderer.set_allocator(malloc, free);

	if (renderer.create_window(800, 600, _T("That's good~"), event_process) == -1) {
        printf("Create window failed!\n");
		goto error;
    }

	my_obj_elements mesh;
	if (my_obj_get_mesh(mesh_file_path, &mesh) == -1) {
        printf("Load mesh from %s failed!\n", mesh_file_path);
		goto error;
    }

	renderer.load_mesh(&mesh);
	camera_reset();
	while (!window_exit) {
		event_dispatch();
		camera_control();
		renderer.clear();
		renderer.draw();
		renderer.refresh();
	}

	my_obj_free_mesh(&mesh);
    renderer.close_window();

	return 0;
error:
	return -1;
}
