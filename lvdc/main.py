import cpu6502
import threading
import time
import sys

from unittest.mock import MagicMock

#
#class Boy():
#    def __init__(self):
#        self.m = MagicMock(return_value=0)
#    def __getattr__(self, name):
#        return self.m
#cpu6502 = Boy()
#

import gi
gi.require_version("Gtk", "3.0")
from gi.repository import Gtk
from gi.repository import Pango
from gi.repository import GObject
from gi.repository import GLib

def update_relay():
    lvdc_widgets[0].set_text('{0:02X}'.format(cpu6502.get_sp()))
    lvdc_widgets[1].set_text('{0:04X}'.format(cpu6502.get_pc()))
    lvdc_widgets[2].set_text('{0:02X}'.format(cpu6502.get_a()))
    lvdc_widgets[3].set_text('{0:02X}'.format(cpu6502.get_x()))
    lvdc_widgets[4].set_text('{0:02X}'.format(cpu6502.get_y()))

    if cpu6502.is_running():
        lvdc_status.set_from_icon_name('gtk-yes', 6)
    else:
        lvdc_status.set_from_icon_name('gtk-no', 6)
    return True

def update_draw():
    life_feed.queue_draw()
    return True

def relay_thread(*args):
    while True:
        time.sleep(1.0 / 30)

x, y = 0, 0
vx, vy = 0, 0
dt = 0.1
g = 9.8
a = 0
fuel = [1.0, 1.0, 1.0]
cmd = 0

def update_telemetry():
    fuel_widgets[0].set_value(fuel[0])
    fuel_widgets[1].set_value(fuel[1])
    fuel_widgets[2].set_value(fuel[2])
    tele_widgets[0].set_text('{:.4f}'.format(x))
    tele_widgets[1].set_text('{:.4f}'.format(y))
    tele_widgets[2].set_text('{:.4f}'.format(vx))
    tele_widgets[3].set_text('{:.4f}'.format(vy))
    burn_widgets[0].set_text('{0:2X}'.format(cmd))
    burn_widgets[1].set_value(cmd)
    return True

def simulation_thread():
    global x,y,vx,vy,dt,g,a,fuel,cmd
    while True:
        cpu6502.write(0xE002, int(fuel[0] * 0xFF))
        cpu6502.write(0xE003, int(fuel[1] * 0xFF))
        cpu6502.write(0xE004, int(fuel[2] * 0xFF))

        time.sleep(0.1)
        cmd = cpu6502.read(0xE010)


        burn_speed = cmd / 255
        a = burn_speed * 10
        fuel[0] = max(fuel[0] - burn_speed * 0.001, 0)
        if fuel[0] == 0:
            a = 0

        vy = vy + (a - g) * dt
        y = y + vy * dt

        if y < 0:
            y = 0
            vy = 0

def draw_cobete(widget, ctx):
    width = 200
    height = 200
    yoffset = 140 - y

    ctx.save()

    ctx.new_path()
    ctx.set_line_width(.5)
    ctx.move_to(0, 190)
    ctx.line_to(90, 190)
    ctx.line_to(90, 199)
    ctx.line_to(110, 199)
    ctx.line_to(110, 190)
    ctx.line_to(200, 190)
    ctx.stroke()

    if a > 0:
        ctx.new_path()
        ctx.set_line_width(1)
        ctx.set_source_rgb(1, 0, 0)
        ctx.arc(100, 51 + yoffset, 8, 0, 3.14159)
        ctx.stroke()

    ctx.new_path()
    ctx.set_source_rgb(0, 0, 0)
    ctx.set_line_width(1)
    ctx.move_to(100, 10 + yoffset)
    ctx.line_to(85,  50 + yoffset)
    ctx.line_to(115, 50 + yoffset)
    ctx.line_to(100, 10 + yoffset)
    ctx.stroke()

    ctx.restore()

def gtk_interface():
    global fuel_widgets, burn_widgets, tele_widgets
    global lvdc_widgets, lvdc_status, life_feed

    def handler(*args, **kwargs):
        cpu6502.set_pc(0x4000)
        cpu6502.run_until_break()

    handlers = {
            'onRunUntilBreak': handler,
            "doDrawCobete": draw_cobete,
    }

    builder = Gtk.Builder()

    builder.add_from_file('lab.glade')

    win = builder.get_object('labwindow')
    win.show_all()

    builder.connect_signals(handlers)

    fuel_widgets = [
            builder.get_object('fuelWidgetStage1'),
            builder.get_object('fuelWidgetStage2'),
            builder.get_object('fuelWidgetStage3'),
    ]
    burn_widgets = [
            builder.get_object('fcBurn'),
            builder.get_object('fcBurnLevelBar'),
    ]
    tele_widgets = [
            builder.get_object('tmRx'),
            builder.get_object('tmRy'),
            builder.get_object('tmVx'),
            builder.get_object('tmVy')
    ]
    lvdc_widgets = [
            builder.get_object('lvdcRegisterSP'),
            builder.get_object('lvdcRegisterPC'),
            builder.get_object('lvdcRegisterA'),
            builder.get_object('lvdcRegisterX'),
            builder.get_object('lvdcRegisterY'),
    ]
    lvdc_status = builder.get_object('lvdcStatusLight')
    life_feed = builder.get_object('lvdcLiveFeed')

    for w in lvdc_widgets:
        w.modify_font(Pango.FontDescription('Monospace'))
    burn_widgets[0].modify_font(Pango.FontDescription('Monospace light'))

    simulation = threading.Thread(target=simulation_thread)
    simulation.daemon = True
    simulation.start()

    relay = threading.Thread(target=relay_thread, args=(lvdc_widgets, lvdc_status))
    relay.daemon = True
    relay.start()

    GLib.idle_add(update_telemetry)
    GLib.idle_add(update_relay)
    GLib.idle_add(update_draw)

    Gtk.main()

def lvdc_main_loop():
    cpu6502.init()

    with open(sys.argv[1], 'rb') as f:
        data = f.read()

    for (index, byte) in enumerate(data):
        cpu6502.write(0x4000 + index, byte)

    cpu6502.set_pc(0x4000)

    gtk_interface()

if __name__ == '__main__':
    lvdc_main_loop()
