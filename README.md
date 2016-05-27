# Ermi-Mapper
Program przywracający georeferencje rastrowym mapom topograficznym.

Notatki:
Warto umieć: QuantumGis i Gdal.
Geo4W - konsola windowsowa dla Gdala.

Gdalinfo wyświetla w formacie wkt.

Polecenia przydatne:
	gdal_translate - pozwala konwertować dane z jednego formatu rastrowego na drugi.
						np z geotifa na jpg/png.
					dodaje plik z danymi aux.xml i trzyma tam metadane.
	gdalinfo - służy do uzyskiwnia informacji o pliku rastrowym.
				wypisze z jakich jest plików. Z grubsza opisuje czy plik poprawny, ile kanałów, georeferencje.
				liczy statystyki i histogramy.
	
	biblioteka ogl: (wektorowe - nie potrzeba nam tego?)
	oglinfo - to samo tylko do wektorowych
	ogl2ogr - tłumaczy dane między danymi wektorowymi
	
georeferencje: obrazek (raster, opis (wysokosć, szerokość obrazu w pikselach), informacje o ukladzie współrzędnych (typ danych) np. epsg,)
sposób przywracania georeferencji - prostszy.
geotransformation - tablica sześciu czego?. To tablica typu X' = ax + by + c Y' = dx +ey +f

Podstawa: 
mapa z georeferencją i mapa bez georeferencji (rózne rozmiary, różne rozdzielczości). Może być ta sama mapa tylko pozbawiona georeferencji.
Skojarz że to są te same mapy.
1.Różny rozmiar
2.Różne skalowanie (wypisz jak zostało przeskalowane)
3.Różny obrót linii geo
4.Mapy mocno przekształcone graficznie.

Widzenie komputerowe - oprogramowanie do analizy i rozpoznawania grafik.
OpenCV
Preferowany model: jak w gdalu: wkt.
