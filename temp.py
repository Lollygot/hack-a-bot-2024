import serial
import struct

PAYLOAD_SIZE = 22

with serial.Serial("COM3", 115200) as ser:
    while True:
        # is blocking with no timeout
        data = ser.read(PAYLOAD_SIZE)

        # use short and float instead of int and double since arduino data type sizes are smaller than standard
        irLeft, irLeftFront, irFront, irRightFront, irRight, x, y, bearing = struct.unpack("<hhhhhfff", data)
        print(f"Data: {data.hex()}")
        print(f"irLeft: {irLeft}")
        print(f"irLeftFront: {irLeftFront}")
        print(f"irFront: {irFront}")
        print(f"irRightFront: {irRightFront}")
        print(f"irRight: {irRight}")
        print(f"x: {x}")
        print(f"y: {y}")
        print(f"Bearing: {bearing}")
