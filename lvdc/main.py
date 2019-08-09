import cpu6502
import threading
import time

def relay_thread(lvdc_widgets):
    while True:
        lvdc_widgets[0].set_text('{0:02X}'.format(cpu6502.get_sp()))
        lvdc_widgets[1].set_text('{0:04X}'.format(cpu6502.get_pc()))
        lvdc_widgets[2].set_text('{0:02X}'.format(cpu6502.get_a()))
        lvdc_widgets[3].set_text('{0:02X}'.format(cpu6502.get_x()))
        lvdc_widgets[4].set_text('{0:02X}'.format(cpu6502.get_y()))
        time.sleep(0.1)

def simulation_thread(fuel_widgets):
    x, y = 0,0
    fuel = [1.0, 1.0, 1.0]

    while True:
        cpu6502.write(0xE000, x)
        cpu6502.write(0xE001, y)
        cpu6502.write(0xE002, int(fuel[0] * 0xFF))
        cpu6502.write(0xE003, int(fuel[1] * 0xFF))
        cpu6502.write(0xE004, int(fuel[2] * 0xFF))

        fuel_widgets[0].set_value(fuel[0])
        fuel_widgets[1].set_value(fuel[1])
        fuel_widgets[2].set_value(fuel[2])

        time.sleep(0.1)
        cmd = cpu6502.read(0xE010)
        if cmd != 0:
            fuel[0] = max(fuel[0] - 0.01, 0)

def gtk_interface():
    import gi
    gi.require_version("Gtk", "3.0")
    from gi.repository import Gtk
    from gi.repository import Pango

    def handler(*args, **kwargs):
        cpu6502.set_pc(0x4000)
        cpu6502.run_until_break()


    handlers = {
            'onRunUntilBreak': handler
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

    lvdc_widgets = [
            builder.get_object('lvdcRegisterSP'),
            builder.get_object('lvdcRegisterPC'),
            builder.get_object('lvdcRegisterA'),
            builder.get_object('lvdcRegisterX'),
            builder.get_object('lvdcRegisterY'),
    ]

    for w in lvdc_widgets:
        w.modify_font(Pango.FontDescription('Monospace'))

    simulation = threading.Thread(target=simulation_thread, args=(fuel_widgets,))
    simulation.start()

    relay = threading.Thread(target=relay_thread, args=(lvdc_widgets,))
    relay.start()

    Gtk.main()

def lvdc_main_loop():

    cpu6502.init()

    with open('run_until_half.bin', 'rb') as f:
        data = f.read()

    for (index, byte) in enumerate(data):
        cpu6502.write(0x4000 + index, byte)

    cpu6502.set_pc(0x4000)

    gtk_interface()

if __name__ == '__main__':

    lvdc_main_loop()
