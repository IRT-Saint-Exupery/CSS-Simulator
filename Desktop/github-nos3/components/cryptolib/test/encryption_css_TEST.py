from cryptography.hazmat.primitives.ciphers.aead import AESGCM

#key      = bytes.fromhex("FEDCBA9876543210FEDCBA9876543210FEDCBA9876543210FEDCBA9876543210")  # 130 id

key      = bytes.fromhex("0d0a5c0dd53472c6bc644100fe70284dc2b08582d9493bb8c8bbd299e780d7ec") # 129 id

iv       = bytes.fromhex("000000000000000000000000")

#data     = bytes.fromhex("1806C00000010000") # CFS NOOP SPP
#data     = bytes.fromhex("1800C0000008000000001E0002002C") # STOP SA SPI 44
#data     = bytes.fromhex("1800C000000800000000190002002C") # EXPIRE SA SPI 44
#data     = bytes.fromhex("1800C000000800000000140002002C") # DELETE SA SPI 44

#data     = bytes.fromhex("1940C000000909003996274217B74741") # set new target as Rome
data     = bytes.fromhex("1940C000000909000000000000000000") # set new target as 0,0
#data     = bytes.fromhex("18C8C00000010C00") # take a photo

aesgcm = AESGCM(key)

aad = bytes.fromhex("0000000000000000000000000000000000000000") # see Crypto_Prepare_TC_AAD

ct = aesgcm.encrypt(iv, data, aad)

#ct = aesgcm.encrypt(iv, data, None)

ct.hex()

print(ct.hex())


#######
def hex_to_vector(hex_string):
    # Remove any leading '0x' if present and convert to uppercase
    hex_string = hex_string.replace("0x", "").upper()
    
    # Ensure the hex string has an even length
    if len(hex_string) % 2 != 0:
        hex_string = '0' + hex_string
    
    # Create a list of integers from the hex string
    vector = [f"0x{hex_string[i:i+2]}" for i in range(0, len(hex_string), 2)]
    
    return vector
    
# Example usage:
hex_input = ct.hex() # Example hexadecimal string
vector_output = hex_to_vector(hex_input)
print(vector_output)    
    
