# Eindopdracht_Embedded_Microcontroller_Applications

We hebben gezien hoe we een MQTT systeem opzetten met je ESP32 en Raspberry Pi en de uitgelezen data kan worden weergegeven in Grafana.
Voor de eindopdracht ga je je bestaande project uitbreiden met drukknoppen, sensoren en actuatoren om zo een mini automatisatie te bekomen.
Je bent volledig vrij in het kiezen van je hardware en aansturing maar ik verwacht een minimum aan complexiteit
Het project bevat minimum:
- ESP32 die zowel publisched EN subscribed op verschillende topics (bidirectioneel)
- Raspberry Pi die zowel publisched EN subscribed op verschillende topics (bidirectioneel)
- Visuele weergave van de data op grafana.
 
Vb: Je ESP32 en Raspberry Pi hebben elks een drukknop en led aangesloten. Als je op de drukknop van de ESP drukt wordt een MQTT publish bericht gestuurd waarop de raspberry pi is ge-subscribed en laat deze een led branden. Je kan op Grafana vervolgens kijken of de led aan of uit is.
Indien je op de drukknop op de Raspberry Pi drukt zal er een MQTT publish gestuurd worden waar de ESP32 op ge-aboneerd is en zo een led laat branden op de ESP32.
Je kan temperatuur uitlezen op de ESP32 en weergeven op een LCD verbonden met de Raspberry pi
 
Dit alles documenteer je en upload je samen met je code op canvas. Je maakt een filmpje waarin je je project voorstelt. ( ! niet enkel de werking tonen maar ook gesproken uitleg bij je project ! )
De documentatie bevat:
- Intro in MQTT en Grafana
- Je project beschrijving (welk probleem ga je oplossen?, waarom, ...)
- Hoe ben je te werk gegaan (Van installeren MQTT tot schrijven van code, testen, ...) 
- Beschrijf de werking van je ESP32 code ( welke stappen doorloopt het programma, ...)
- Beschrijf de werking van je Raspberry Pi code ( welke stappen doorloopt het programma, ...)
- Hoe controleer je de werking van MQTT (worden pakketten doorgestuurd, ontvangen, ...)

Filmpje Werking
https://www.youtube.com/watch?v=sMnkvHMmoqA

