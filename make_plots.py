#!/usr/bin/python3
# -*- coding: utf-8 -*-
import matplotlib.pyplot as plot
import multiprocessing as mp
title='%s'
# open needed files
with open(%s) as f:  # signal data open
    signal = (f.read().splitlines())
with open(%s) as f:  # macd data open
    macd = (f.read().splitlines())
with open(%s) as f:  # rate data open
    rate = (f.read().splitlines())


# Draw macd and signal plot
def draw_signal_macd_plot(signal, macd):
    plot.figure(figsize=(20, 5))
    plot.margins(0, 0.2)
    plot.axhline(0, color='gray', linestyle='--', linewidth=0.1)
    plot.plot(signal, linewidth=0.5, color='red', label="Signal")
    plot.plot(macd, linewidth=0.5, color='blue', label="MACD")
    plot.xlabel('Day')
    plot.ylabel('Value')
    plot.legend()
    plot.title(title+': SIGNAL & MACD')
    plot.savefig(%s, dpi = 600, bbox_inches = 'tight')  # save macd_signal plot
    plot.cla()

# Draw rate plot
def draw_rate_plot(rate):
    plot.figure(figsize=(20, 5))
    plot.margins(0, 0.2)
    plot.xlabel('Day')
    plot.ylabel('Value')
    plot.plot(rate, linewidth=0.5, color='green', label=title)
    plot.legend()
    plot.title(title)
    plot.savefig(%s, dpi = 600, bbox_inches = 'tight')  # save rate plot


# create processes
draw_signal_macd_plot_process = mp.Process(target=draw_signal_macd_plot, args=(signal, macd))
draw_rate_plot_process = mp.Process(target=draw_rate_plot, args=(rate,))


# run processes
draw_rate_plot_process.start()
draw_signal_macd_plot_process.start()


#wait for processes
draw_rate_plot_process.join()
draw_signal_macd_plot_process.join()