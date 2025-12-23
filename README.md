> I'd need more information on how to replicate your project

Hello, sorry did miss your question.

It is a project based on Arduino-like platform, mine uses cheap Arduino Uno but other flavors should work

There are schematics in the pdf file. It is basically a boost dc-dc converter at anode, coupled with a microcurrent regulator at cathode,each regulated by software (adc feedback), the most important being microcurrent feedback (measuring small <1v voltage drop through big value resistor).

The current is integrated into Coulombs (moles of électrons) and translated to ppm by Arduino software (the .ino file) and stops at #defined ppm level . Many #defines on top of that .ino files for jar volume, required ppm, max current, pin used, resistors values, etc .. flash after change with Arduino ide at ease.

The log.py, by opening and reading the Arduino serial port (though usb) on host computer, starts/reset the process on Arduino side, and records all data on current folder as timestamped .csv file. Run it in terminal, ms windows untested.

Water resistance, voltage, current, ppm, etc.. Are logged in that csv file .

The plot.py file then can open such csv file and show nice graph on browser, allowing to see inflexion points on resistance (hints at colloïds size and ionic form concentration) etc ..
It uses a magnet sticked on a computer fan spinning at low voltage, underneath the jar, with magnetic stirer stick, kinda low tech diy magnetic stirrer. BUT found it needs to be blocked on the side of the jar, barely vibrating rather than spinning . That is enough agitation IME , more would produce undesirable observed bigger particules, my guess is that there is a probabilistic thing on ion encounters, depending strong on agitation.

All in all, a rather feature full and flawless, yet ultracheap colloïdal silver generator, gave great services to my family and friends. 

The most expensive being the 99.99 silver coin at Anode (one once Canadian dollar) . Cathode is a simple graphite stick which i clean as it slowly coats with silver.

Use with ultrapure / distilated water, typically measured at 50+ kohm when the coin is spaced 3-4cm apart cathode. Filter with coffee paper filter
 
Recomended not go above 13ppm or silver precipitate appear. 4-7ppm is fine too. A laser in real dark place through it reveal the coloids. It's almost a panacea, but as it brings strong detoxication as a cure, recommended to drink a lot of water

Feel free to leave comment if you like, or need more info.

Olivier "Fines Têtes"
