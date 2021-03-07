import serial
import time
from inputimeout import inputimeout, TimeoutOccurred

opcion1 = ''
puerto = 'COM11'
velocidad = 115200


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
                nuevocomando = inputimeout(prompt='', timeout=5)
            except TimeoutOccurred:
                nuevocomando = ''
            # lee puerto y muestra lectura
            lines = ser.readlines()
            if guardar == '': guardando = False
            else: guardando = True
            
            for line in lines:
                if guardando:
                    guardar = guardar + line.decode("utf8", 'ignore')
                    print(line)

                if line.startswith(b'Ciclo Iniciado'):
                    guardando = True
                    hora_inicio = str(time.time())
                    guardar = hora_inicio + '\n'
                    print('\nEmpieza a guardar\n')
                elif line.startswith(b'Demora total'): 
                    guardando = False
                    print('\nTermina de guardar\n')
                    # escribe la variable guardar en un nuevo archivo
                    my_file = open(hora_inicio + '.txt', 'w')
                    my_file.writelines(guardar) #Write multiple lines
                    my_file.close()
                    print('Creado archivo ' + hora_inicio + '.txt')
                    nuevocomando = 'q'
                    
            
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
        abriryenviar(comando, '')

    
