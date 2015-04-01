# Sistema di deumidifcazione ecologica

## Principio di funzionamento

Il sistema di deumidifcazione funziona solo in climi dove l'umidità dell'aria relativa diurna è mediamente accettabile. L'aria esterna verrà utilizzata come il mezzo prosciugante di un ambiente chiuso.

Data una soglia di umidità massima tollerata, si confronta l'umidità relativa istantanea ogni 3 secondi all'interno dell'ambiente (in questo caso un archivio cartaceo).

Se l'umidità rilevata è maggiore di quella tollerata e se l'umidità misurata all'esterno è più bassa di quella interna (cioè l'aria fuori è più secca) allora si azionano le ventole per il ricambio d'aria pompando aria secca all'interno dall'alto e succhiando fuori l'aria umida dal basso.

Con i pulsanti "UP" e "DOWN" si ciclano le schermate:
* "valori di umidità interno/esterno e soglia", 
* "valori esterni di umidità e temperatura", 
* "valori interni di umidità e temperatura", 
* "soglia di umisità".
Con il pulsante "Select" si va alla schermata di 
* "impostazione soglia di umisità", 
qui con i pulsanti "UP" e "DOWN" si imposta a scatti di 5 la soglia in valori percentuali (minimo 5%, massimo 95%).

Il valore è memorizzato nella EEPROM in modo da essere mantenuto anche in caso di mancanza di alimentazione.

Se la lettura del valore delle sonde non avviene , i valori impossibili e negativi ritornati hanno questi significati:
* -1 = "Checksum error" 
* -2 = "Time out error" (sicuri che la sonda sia collegata?)
* -3 = "Unknown error" 

## Impostazione di massima del sistema
* Il locale deve essere ermeticamente chiuso
* Il sensore di umidità esterno va posto possibilmente in alto e nello stesso punto va installata una ventola con flusso da fuori a dentro. Dopo un periodo di osservazione di qualche mese, statisticamente il punto in alto scelto è sempre stato più secco di quello in basso.
* Il sensore di umidità interno va posto possibilmente in basso e nello stesso punto va installata una ventola con flusso da dentro a fuori.

## Componenti

* un arduino (testato duemilanove)
* 2 sensori DHT11 o DHT22 (configurare nel codice il tipo di sensore)
* 2 relay (usati 2 moduli della keyes, già compresi di resistenza, led e diodo di protezione)
* 2 ventole (recuperate 2 CF12825MS)
* un DF ROBOT LCD shield (con pulsanti su pin analogico A0)
* alimentatore 12Vcc (sia per alimentare le ventole tramite i relay, sia per alimentare il modulo DC-DC che alimeta l'arduino)
* un modulo DC-DC 12-5 (per alimentare l'arduino)
* scatola gewiss, cavi, silicone, etc

![schema di collegamento](https://github.com/audric/arduino-humidity-eco-ventilation/raw/master/schema.png)

## Calcoli vari

Il locale da deumidificare (l'archivio) è circa 3x3x3metri, cioè 27 metri cubi. La ventola utilizzata ha un flusso di 30,2 piedi cubici al minuto pari a 0,855169 metri cubi al minuto. Il totale ricambio d'aria si avrà quindi in 31,5 minuti.

Ogni ventola consuma 0,19A a 12Vdc, quindi insieme circa 0,4A @12Vdc.

L'arduino consuma circa 200mA @ 5Vdc, per la conversione DC-DC, facciamo finta che renda solo il 50%, quindi 0,4A @12Vdc.

In totale siamo a 0,8A @12VDC. Un alimentatore da minimo 10W dovrebbe andar bene.


# Dipendendenze
Questo codice utilizza le librerie:
* LiquidCrystal
* ![OSEPP LCDKeypad](http://osepp.com/products/shield-arduino-compatible/16x2-lcd-display-keypad-shield/)
* EEPROM
* DHT
