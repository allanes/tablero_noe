import serial
import time
from inputimeout import inputimeout, TimeoutOccurred
from datetime import datetime

# Configuración de puerto y velocidad por defecto
puerto = 'COM11'
velocidad = 115200
# Configuracion para el control de creacion de archivos
clave_inicio = b'Ciclo Iniciado'
clave_fin = b'Fin del proceso'
# Tiempo maximo de espera al usuario (de forma cíclica) en segundos
# El micro deja de mostrar los datos que recibe y espera
esperar_usuario = 3


opcion1 = ''
aux_mal_levantada = ''
def abriryenviar(cmd, guardar):
    volveraentrar = False
    with serial.Serial(puerto, velocidad, timeout=1) as ser:
        # espera inicializacion
        time.sleep(3)
        # envia comando
        cmd = cmd + '\n'
        ser.write(bytes(cmd, "utf8"))
        # loop de lectura serie y lectura usuario
        nuevocomando = ''
        print('>>')
        
        # evalua y sigue leyendo
        while (nuevocomando == ''):
            # busca entrada de usuario
            try:
                nuevocomando = inputimeout(prompt='', timeout=esperar_usuario)
            except TimeoutOccurred:
                nuevocomando = ''
            # lee puerto y muestra lectura
            lines = ser.readlines()
            if guardar == '': guardando = False
            else: guardando = True
            
            for line in lines:
                if line.startswith(clave_inicio):
                    guardando = True
                    now = datetime.now()
                    hora_inicio = now.strftime("%Y%m%d%H%M%S")
                    continue
                elif line.startswith(clave_fin): 
                    guardando = False
                    # escribe la variable guardar en un nuevo archivo
                    my_file = open(hora_inicio + '.txt', 'w')
                    my_file.write(guardar) #Write multiple lines
                    my_file.close()
                    print('Creado archivo ' + hora_inicio + '.txt')
                    nuevocomando = 'q'
                if guardando:
                    aux = line.decode("utf8", 'ignore')
                    if (aux.startswith('Pieza mal levantada')):
                        if (aux_mal_levanta is not aux):
                            aux_mal_levantada = aux
                            print(aux_mal_levantada)
                    else:
                        guardar = guardar + aux.strip("\n-")
                    # print(line)
            
        if nuevocomando == 'STOP' or 'q':
            print('\nESCRIBE STOP\n')
            ser.write(b'STOP\n')
        else:
            print('\nVUELVE A ENTRAR\n')
            volveraentrar = True
    if volveraentrar == True:
        abriryenviar(nuevocomando, guardar)





while(opcion1 != 'q'):
    print('Ingrese:\n   ENTER para comando START\n   q, para salir\n   p, para seleccionar puerto \n   ó el comando a enviar')
    print('Puerto: ' + puerto)
    comando = ''
    opcion1 = ''
    opcion1 = input('\n>>')
    if opcion1 == '':
        comando = 'START'
        abriryenviar(comando, '')
    elif opcion1 == 'q':
        comando = ''
    elif opcion1 == 'p':
        print('Escriba el puerto: ')
        puerto = input()
    else:
        comando = opcion1
        abriryenviar(comando, '')

    
