# arduino-humidity-eco-ventilation
Controllo umidità e deumidificatore ecologico con arduino, 2 sensori DHT11 o DHT22, 2 relay, 2 ventole e un DF ROBOT LCD shield con pulsanti su pin analogico A0.

# Principio di funzionamento
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

# Impostazione di massima del sistema
* Il locale deve essere ermeticamente chiuso
* Il sensore di umidità esterno va posto possibilmente in alto e nello stesso punto va installata una ventola con flusso da fuori a dentro. Dopo un periodo di osservazione di qualche mese, statisticamente il punto in alto scelto è sempre stato più secco di quello in basso.
* Il sensore di umidità interno va posto possibilmente in basso e nello stesso punto va installata una ventola con flusso da dentro a fuori.

![schema di collegamento](https://github.com/audric/arduino-humidity-eco-ventilation/raw/master/schema.png)

# Dipendendenze
Questo codice utilizza le librerie:
* LiquidCrystal
* ![OSEPP LCDKeypad](http://osepp.com/products/shield-arduino-compatible/16x2-lcd-display-keypad-shield/)
* EEPROM
* DHT
