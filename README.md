# Upravljanje-signalima
Ovaj kod implementira jednostavan sustav za obradu signala u C-u, koji rješava problem upravljanja prekidima s različitim prioritetima. Koristi se za hvatanje i obradu tri različita signala: SIGINT, SIGTERM i SIGUSR1, gdje svaki signal ima svoj prioritet (SIGINT ima najviši prioritet, zatim SIGTERM, a SIGUSR1 najniži).

Program bilježi vrijeme od početka rada i koristi tu informaciju pri ispisu poruka. Kada signal stigne, on se ili obrađuje odmah ili se pamti ako trenutni prioritetni signal ima viši prioritet. Kroz funkciju `obradi_signal` program simulira obradu signala, uz istovremeno bilježenje informacija o stanju sustava (trenutni prioritetni signal, stog prekida, itd.).

Cilj je pokazati kako upravljati signalima na način da se višestruki signali različitih prioriteta mogu ispravno obraditi i kako se sustav vraća u normalno stanje nakon obrade svakog signala.
