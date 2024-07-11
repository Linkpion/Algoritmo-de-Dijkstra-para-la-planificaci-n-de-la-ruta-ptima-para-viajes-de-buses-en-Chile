#Código usado para obtener los datos de recorridos.cl mediante WebScrapping
#Hecho por Cristóbal Artus, Diego Muñoz, Esteban Rojas, Benjamín Villablanca 

from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.common.exceptions import TimeoutException, NoSuchElementException
from openpyxl import Workbook
import time
import unidecode

#Funcion que filtra ciudades internacionales o Aereopuertos
def verificar_ciudad(ciudad):
    if(ciudad.endswith("(Argentina)") or ciudad.endswith("(Perú)") or ciudad.endswith("(Bolivia)") or ciudad.endswith("(SCL)") or ciudad.endswith(")")):
        return False
    return True

#Convertir texto a formato del link
def link(texto):
    # Convertir todo el texto a minúsculas
    texto = texto.lower()
    
    # Reemplazar los espacios por guiones
    texto = texto.replace(" ", "-")
    
    # Eliminar los acentos, excepto para 'ü'
    resultado = []
    for caracter in texto:
        if caracter == 'ü':
            resultado.append(caracter)
        else:
            resultado.append(unidecode.unidecode(caracter))
    
    return ''.join(resultado)

if __name__ == '__main__':
    book = Workbook()
    sheet=book.active
    casilla=2
    ciudad = 1
    logs=open('errors.txt','w')
    # Configura el controlador
    driver = webdriver.Chrome()

    # Abre la página web
    driver.get('https://www.recorrido.cl/es')

    # Maximiza la ventana del navegador
    driver.maximize_window()

    #Localizado lista de ciudades
    WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, '/html/body/main/div/div[1]/form/div[1]/div[1]/div/div/div[2]/div/div/div[2]/div')))
    ciudades_rutas=driver.find_element(By.XPATH , '/html/body/main/div/div[1]/form/div[1]/div[1]/div/div/div[2]/div/div/div[2]/div')

    #Guardado de las ciudades
    ciudades_hijos=ciudades_rutas.find_elements()
    cantidad_ciudades = len(ciudades_hijos)
    print(ciudades_hijos)

    for i in range(1,cantidad_ciudades):
        
        #Guardado de la ciudad de origen I
        driver.find_element(By.XPATH , '/html/body/main/div/div[1]/form/div[1]/div[1]/div/div/div[2]/div/div/div[1]').click()
        ciudad_html=driver.find_element(By.XPATH , '/html/body/main/div/div[1]/form/div[1]/div[1]/div/div/div[2]/div/div/div[2]/div/div[{0}]'.format(i))
        ciudad_origen=ciudad_html.text

        if(verificar_ciudad(ciudad_origen)):
            print(ciudad_origen)

            ciudad_html.click()
            time.sleep(5)
            #Guardado de las posibles ciudades de destino
            driver.find_element(By.XPATH , '/html/body/main/div/div[1]/form/div[1]/div[2]/div/div/div[2]/div/div/div[1]').click()
            ciudades_destino=driver.find_element(By.XPATH , '/html/body/main/div/div[1]/form/div[1]/div[2]/div[1]/div/div[2]/div/div/div[2]/div')
            print(ciudades_destino)
            destino_hijos = ciudades_destino.find_elements(By.XPATH , './div')
            destinos=list()

            #Añadir ciudades de destino a la lista de destinos
            for j in range(1,len(destino_hijos)+1):           
                destino=driver.find_element(By.XPATH , '/html/body/main/div/div[1]/form/div[1]/div[2]/div[1]/div/div[2]/div/div/div[2]/div/div[{0}]'.format(j)).text
                destinos.append(destino)

            for destino in destinos:
                if(verificar_ciudad(destino)):
                    #Busqueda por cada fecha (24, 25 y 26 de Junio en nuestro paper)
                    fechas = ['24','25','26']
                    for fecha in fechas:
                        try:
                            driver.get('https://www.recorrido.cl/es/bus/{0}/{1}/{2}-06-2024'.format(link(ciudad_origen),link(destino),fecha))

                            try:
                                # Espera explícita para que el elemento esté presente en el DOM
                                time.sleep(2)
                                elemento = WebDriverWait(driver, 10).until(
                                    EC.presence_of_element_located((By.XPATH, "/html/body/main/div/div[1]/div[3]/div[3]/form/div[5]/div/div/div/div/p"))
                                )
                                print("No existen viajes.")
                                break
                            except TimeoutException:
                                print("El elemento no existe en la página.")

                                # Espera a que la página cargue completamente
                                WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, '/html/body/main/div/div[1]/div[3]/div[3]/form/div[5]/div/div')))

                                # Encuentra el div de la primera empresa
                                div = driver.find_element(By.XPATH, '/html/body/main/div/div[1]/div[3]/div[3]/form/div[5]/div/div')

                                # Encuentra todos los elementos hijos del div
                                elementos_hijos = div.find_elements(By.XPATH, './div')

                                # Obtiene la cantidad de elementos hijos
                                cantidad_elementos = len(elementos_hijos)

                                for k in range(1,cantidad_elementos+1):
                                    #Localiza cada empresa
                                    elemento = driver.find_element(By.XPATH, '/html/body/main/div/div[1]/div[3]/div[3]/form/div[5]/div/div/div[{0}]'.format(k))
                                    elemento.click()


                                    nombre_empresa=driver.find_element(By.XPATH, '/html/body/main/div/div[1]/div[3]/div[3]/form/div[5]/div/div/div[{0}]/div[1]/div[2]/div/div[1]/p/strong'.format(k)).text
                                    print(nombre_empresa)
                                    time.sleep(5)

                                    # Encuentra el div en cuestión
                                    div_a = driver.find_element(By.XPATH, '/html/body/main/div/div[1]/div[3]/div[3]/form/div[5]/div/div/div[{0}]/div[2]/div/ul'.format(k))

                                    # Encuentra todos los elementos hijos del div
                                    elementos_hijos_a = div_a.find_elements(By.XPATH, './li')
                                    # Obtiene la cantidad de elementos hijos
                                    cantidad_elementos_a = len(elementos_hijos_a)
                                    for j in range(1,cantidad_elementos_a+1):
                                        #                                     /html/body/main/div/div[1]/div[3]/div[3]/form/div[5]/div/div/div[i]/div[2]/div/ul/li[j]
                                        #Se extraen los elementos que se solicitan y se guardan en excel
                                        hora = driver.find_element(By.XPATH, '/html/body/main/div/div[1]/div[3]/div[3]/form/div[5]/div/div/div[{0}]/div[2]/div/ul/li[{1}]/div[2]/div/div[1]/div[1]/div[1]/p/strong/span[3]'.format(k,j)).text
                                        duracion = driver.find_element(By.XPATH, '/html/body/main/div/div[1]/div[3]/div[3]/form/div[5]/div/div/div[{0}]/div[2]/div/ul/li[{1}]/div[2]/div/div[3]/p/span/span'.format(k,j)).text
                                        precio = driver.find_element(By.XPATH, '/html/body/main/div/div[1]/div[3]/div[3]/form/div[5]/div/div/div[{0}]/div[2]/div/ul/li[{1}]/div[2]/div/div[4]/p/span[1]'.format(k,j)).text
                                        print("Hora: {0} Duracion: {1} Precio {2}".format(hora,duracion,precio))
                                        sheet['A'+str(casilla)]=ciudad_origen
                                        sheet['B'+str(casilla)]=destino
                                        sheet['C'+str(casilla)]=nombre_empresa
                                        sheet['D'+str(casilla)]="{0}-06-2024".format(fecha)
                                        sheet['E'+str(casilla)]=hora
                                        sheet['F'+str(casilla)]=duracion
                                        sheet['G'+str(casilla)]=precio
                                        book.save('database.xlsx')
                                        casilla+=1
                        except:
                            #Errores no localizados
                            logs.write("{0} - {1}\n".format(ciudad_origen,destino))
                            break
        #Regreso a la página de incio
        driver.get('https://www.recorrido.cl/es')
        WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, '/html/body/main/div/div[1]/form/div[1]/div[1]/div/div/div[2]/div/div/div[2]/div')))
        ciudad+=1

    # Cierra el navegador
    driver.quit()
    logs.close()