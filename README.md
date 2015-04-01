# arduino-humidity-eco-ventilation
Controllo umidità e deumidificatore ecologico con arduino, 2 sensori DHT11 o DHT22, 2 relay, 2 ventole e un DF ROBOT LCD shield con pulsanti su pin analogico A0.

# Principio di funzionamento
Data una soglia di umidità massima tollerata, si confronta l'umidità relativa istantanea ogni 3 secondi all'interno dell'ambiente (in questo caso un archivio cartaceo). Se l'umidità rilevata è maggiore di quella tollerata e se l'umidità misurata all'esterno è più bassa di quella interna (cioè l'aria fuori è più secca) allora si azionano le ventole per il ricambio d'aria pompando aria secca all'interno dall'alto e succhiando fuori l'aria umida dal basso.

# Impostazione di massima del sistema
* Il locale deve essere ermeticamente chiuso
* Il sensore di umidità esterno va posto possibilmente in alto e nello stesso punto va installata una ventola con flusso da fuori a dentro. Dopo un periodo di osservazione di qualche mese, statisticamente il punto in alto scelto è sempre stato più secco di quello in basso.
* Il sensore di umidità interno va posto possibilmente in basso e nello stesso punto va installata una ventola con flusso da dentro a fuori.

# Dipendendenze
Questo codice utilizza la libreria OSEPP LCDKeypad  http://osepp.com/products/shield-arduino-compatible/16x2-lcd-display-keypad-shield/
