#!/usr/bin/python
# coding: utf-8

import plotly
from plotly.graph_objs import Scatter, Layout
import plotly.plotly as py
import plotly.graph_objs as go

import datetime
import csv
import math

import sys

lasttime=0
firsttime=0

interval_min = 20

def dt(u): return datetime.datetime.utcfromtimestamp(u)


csvfiles = [
sys.argv[1],
]


bytime = {}
data = []
times = []
x = []
inserted = 0

def insert_cold(cold):
    global firsttime,lasttime,data,bytime,times,x,inserted
    it_time = int(float(cold['time']))
    bytime[it_time]=cold
    times.append(it_time)
    x.append(dt(it_time))

    curtime=long(float(cold['time'])) * 1000
    if firsttime==0:
        firsttime=long(float(cold['time'])) * 1000
    else:
        if long(float(cold['time'])) * 1000 < firsttime:
            firsttime=long(float(cold['time'])) * 1000
    
    if lasttime==0:
        lasttime=long(float(cold['time'])) * 1000
    else:
        if long(float(cold['time'])) * 1000 > lasttime:
            lasttime=long(float(cold['time'])) * 1000

    inserted += 1


def read_data(csvfile):
        cr = csv.reader(open(csvfile,"rb"))
	rownum = 0
#	data = []
#	times = []
#	x = []
#        bytime = {}
        last_time = 0
        last_cold = {}
        print interval_min
        import pudb
#        pudb.set_trace()
	for row in cr:
	    if rownum == 0:
		header_tmp = row
                header = []
                for key in header_tmp:
                    header.append(key)
                    last_key = key
	    else:
		colnum = 0
		cold = {}
		for col in row:
                    try:
		        cold[header[colnum]] = col
		        colnum += 1
                    except IndexError as e:
                        print "IndexError on file", filename, "at row",rownum,':',e
                        pass
		data.append(cold)
                if 'time' in cold and cold['time']!= '' and cold['time']!= '' and int(float(cold['time'])):
                    it_time = int(float(cold['time']))
                    if last_time > 0:
                        if it_time - last_time > interval_min:
                            if not last_time in times:
                                insert_cold(last_cold)
                    if it_time - (lasttime/1000) > interval_min:
                        insert_cold(cold)

                    last_time = it_time
                    last_cold = cold

	#    print repr(cold)
	    rownum += 1
	#return (header,data,bytime,times,x)
	return


#(header,data,bytime,times,x)=read_data('climate_log.csv')
#print header

for filename in csvfiles:
    read_data(filename)

print "Considered",inserted,"rows."

leg_vars = {
  'outv_avg': {
     'name': 'Voltage',
     'legendgroup' : 'inside',
   },
  'ma': {
     'name': 'Current',
     'legendgroup' : 'inside',
   },
  'ohms': {
     'name': 'Resistance',
     'legendgroup' : 'inside',
   },
  'ppm': {
     'name': 'ppm',
     'legendgroup' : 'inside',
   },
}

def trace_var(var,coeff=1.0):
    y=[]
    for t in times:
        if var in bytime[t] and bytime[t][var] and float(bytime[t][var]) >0.001:
            value = float(bytime[t][var]) * float(coeff)
            if value > 100 or value < -50:
                print "Overflow on ",var,"at",t,dt(t)
                y.append(None)
            else:
                y.append(value)
        else:
            y.append(None)
    return y

print firsttime, lasttime

def time_plot(data):
  plotly.offline.plot({
    "data": data,
    "layout": Layout(title="Climact Viewer", xaxis=dict(
    range=[firsttime,lasttime],
    rangeselector=dict(
            buttons=list([
        dict(count=1,
                     label='1d',
                     step='day',
                     stepmode='backward'),

        dict(count=1,
                     label='1w',
                     step='week',
                     stepmode='backward'),

                dict(count=1,
                     label='1m',
                     step='month',
                     stepmode='backward'),
                dict(count=6,
                     label='6m',
                     step='month',
                     stepmode='backward'),
                dict(count=1,
                    label='YTD',
                    step='year',
                    stepmode='todate'),
                dict(count=1,
                    label='1y',
                    step='year',
                    stepmode='backward'),
                dict(step='all')
            ])
        ),
        rangeslider=dict(),
        type='date',
        title='Time',  
    ),
    yaxis=dict(
    title='Volts / .1mA / kohms / ppm'
      )
    )
})


pdata=[]
hdata=[]
adata=[]

adata.append(Scatter(x=x, y=trace_var('ma',10.0), legendgroup=leg_vars['ma']['legendgroup'], name= leg_vars['ma']['name']))
adata.append(Scatter(x=x, y=trace_var('outv_avg'), legendgroup=leg_vars['outv_avg']['legendgroup'], name= leg_vars['outv_avg']['name']))
adata.append(Scatter(x=x, y=trace_var('ohms',0.0001), legendgroup=leg_vars['ohms']['legendgroup'], name= leg_vars['ohms']['name']))
adata.append(Scatter(x=x, y=trace_var('ppm'), legendgroup=leg_vars['ppm']['legendgroup'], name= leg_vars['ppm']['name']))


time_plot(adata)
