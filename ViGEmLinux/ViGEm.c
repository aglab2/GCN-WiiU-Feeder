#include "Compat.h"
#include "ViGEm/Client.h"

#include <fcntl.h>
#include <linux/uinput.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct _VIGEM_CLIENT_T
{
    // TODO: move to target?
    int fd;
} VIGEM_CLIENT;

PVIGEM_CLIENT vigem_alloc()
{
    VIGEM_CLIENT *client = (VIGEM_CLIENT *)malloc(sizeof(VIGEM_CLIENT));
    client->fd = -1;

    return client;
}

static void setup_abs(int fd, unsigned chan, int min, int max)
{
    ioctl(fd, UI_SET_ABSBIT, chan);
    struct uinput_abs_setup s = {
        .code = chan,
        .absinfo = {.minimum = min, .maximum = max},
    };

    ioctl(fd, UI_ABS_SETUP, &s);
}

VIGEM_ERROR vigem_connect(PVIGEM_CLIENT vigem)
{
    if (vigem->fd != -1)
        return VIGEM_ERROR_ALREADY_CONNECTED;

    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (-1 == fd)
        return VIGEM_ERROR_BUS_NOT_FOUND;

    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, BTN_A);
    ioctl(fd, UI_SET_KEYBIT, BTN_B);
    ioctl(fd, UI_SET_KEYBIT, BTN_X);
    ioctl(fd, UI_SET_KEYBIT, BTN_Y);
    ioctl(fd, UI_SET_KEYBIT, BTN_TL);
    ioctl(fd, UI_SET_KEYBIT, BTN_TR);
    ioctl(fd, UI_SET_KEYBIT, BTN_START);
    ioctl(fd, UI_SET_KEYBIT, BTN_SELECT);
    ioctl(fd, UI_SET_KEYBIT, BTN_THUMBL);
    ioctl(fd, UI_SET_KEYBIT, BTN_THUMBR);
    ioctl(fd, UI_SET_KEYBIT, BTN_DPAD_UP);
    ioctl(fd, UI_SET_KEYBIT, BTN_DPAD_DOWN);
    ioctl(fd, UI_SET_KEYBIT, BTN_DPAD_LEFT);
    ioctl(fd, UI_SET_KEYBIT, BTN_DPAD_RIGHT);

    ioctl(fd, UI_SET_EVBIT, EV_ABS);
    setup_abs(fd, ABS_X, -32768, 32767);
    setup_abs(fd, ABS_Y, -32768, 32767);
    setup_abs(fd, ABS_Z, 0, 255);
    setup_abs(fd, ABS_RX, -32768, 32767);
    setup_abs(fd, ABS_RY, -32768, 32767);
    setup_abs(fd, ABS_RZ, 0, 255);

    vigem->fd = fd;
    return VIGEM_ERROR_NONE;
}

void vigem_disconnect(PVIGEM_CLIENT vigem)
{
    close(vigem->fd);
    vigem->fd = -1;
}

typedef struct _VIGEM_TARGET_T
{
    int added;
} VIGEM_TARGET;

PVIGEM_TARGET vigem_target_x360_alloc(void)
{
    VIGEM_TARGET *target = (PVIGEM_TARGET)malloc(sizeof(VIGEM_TARGET));
    target->added = 0;
    return target;
}

void vigem_target_free(PVIGEM_TARGET target)
{
    free(target);
}

VIGEM_ERROR vigem_target_add(PVIGEM_CLIENT vigem, PVIGEM_TARGET target)
{
    if (target->added)
        return VIGEM_ERROR_ALREADY_CONNECTED;

    int fd = vigem->fd;

    struct uinput_setup setup =
        {
            .name = "ViGEm",
            .id =
                {
                    .bustype = BUS_USB,
                    .vendor = 0x3,
                    .product = 0x3,
                    .version = 2,
                }};

    ioctl(fd, UI_DEV_SETUP, &setup);
    ioctl(fd, UI_DEV_CREATE);

    target->added = 1;
    return VIGEM_ERROR_NONE;
}

VIGEM_ERROR vigem_target_remove(PVIGEM_CLIENT vigem, PVIGEM_TARGET target)
{
    if (!target->added)
        return VIGEM_ERROR_TARGET_NOT_PLUGGED_IN;

    ioctl(vigem->fd, UI_DEV_DESTROY);
    target->added = 0;
    return VIGEM_ERROR_NONE;
}

VIGEM_ERROR vigem_target_x360_update(
    PVIGEM_CLIENT vigem,
    PVIGEM_TARGET target,
    XUSB_REPORT report)
{
    report.sThumbLY = -1-report.sThumbLY;
    report.sThumbRY = -1-report.sThumbRY;

    struct input_event ev[24];

    int counter = 0;
#define ADD_BUTTON(x_btn, ev_btn) \
    ev[counter++] = (struct input_event) { \
        .type = EV_KEY, \
        .code = ev_btn, \
        .value = !!(report.wButtons & x_btn), \
    }

    ADD_BUTTON(XUSB_GAMEPAD_A, BTN_A);
    ADD_BUTTON(XUSB_GAMEPAD_B, BTN_B);
    ADD_BUTTON(XUSB_GAMEPAD_X, BTN_X);
    ADD_BUTTON(XUSB_GAMEPAD_Y, BTN_Y);
    ADD_BUTTON(XUSB_GAMEPAD_DPAD_UP, BTN_DPAD_UP);
    ADD_BUTTON(XUSB_GAMEPAD_DPAD_DOWN, BTN_DPAD_DOWN);
    ADD_BUTTON(XUSB_GAMEPAD_DPAD_LEFT, BTN_DPAD_LEFT);
    ADD_BUTTON(XUSB_GAMEPAD_DPAD_RIGHT, BTN_DPAD_RIGHT);
    ADD_BUTTON(XUSB_GAMEPAD_START, BTN_START);
    ADD_BUTTON(XUSB_GAMEPAD_BACK, BTN_SELECT);
    ADD_BUTTON(XUSB_GAMEPAD_LEFT_THUMB, BTN_THUMBL);
    ADD_BUTTON(XUSB_GAMEPAD_RIGHT_THUMB, BTN_THUMBR);
    ADD_BUTTON(XUSB_GAMEPAD_LEFT_SHOULDER, BTN_TL);
    ADD_BUTTON(XUSB_GAMEPAD_RIGHT_SHOULDER, BTN_TR);
    ADD_BUTTON(XUSB_GAMEPAD_GUIDE, BTN_MODE);

#define ADD_STICK(x_val, ev_code) \
    ev[counter++] = (struct input_event) { \
        .type = EV_ABS, \
        .code = ev_code, \
        .value = report.x_val, \
    }

    ADD_STICK(sThumbLX, ABS_X);
    ADD_STICK(sThumbLY, ABS_Y);
    ADD_STICK(sThumbRX, ABS_RX);
    ADD_STICK(sThumbRY, ABS_RY);
    ADD_STICK(bLeftTrigger, ABS_Z);
    ADD_STICK(bRightTrigger, ABS_RZ);

#undef ADD_BUTTON
#undef ADD_STICK

    ev[counter++] = (struct input_event) {
        .type = EV_SYN,
        .code = SYN_REPORT,
        .value = 0,
    };

    ssize_t sz = write(vigem->fd, &ev, sizeof(ev));
    return sz == sizeof(ev) ? VIGEM_ERROR_NONE : VIGEM_ERROR_BUS_ACCESS_FAILED;
}

void vigem_free(PVIGEM_CLIENT vigem)
{
    close(vigem->fd);
    free(vigem);
}
