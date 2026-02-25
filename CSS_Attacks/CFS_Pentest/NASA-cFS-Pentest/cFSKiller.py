

def banner():
    print("")
    print("      ________  __ ________   __   _______    {v1.0}")
    print(" ____/ __/ __/ / //_/  _/ /  / /  / __/ _ \   Adrian Schalk")
    print("/ __/ _/_\ \  / ,< _/ // /__/ /__/ _// , _/   Luke Brodnik")
    print("\__/_/ /___/ /_/|_/___/____/____/___/_/|_|    https://github.com/0xsninja/NASA-cFS-Pentest")
    print("")
    print("")
    print("Welcome to cFS KILLER, a collection of attacks usable on NASA's cFS")
    print()
    





def main():
    banner()
    userInput()

def userInput():
    call = input("cFSK> ")
    if call == ("q"):
        print("Quitting")
        return
    elif call == "h":
        print("Here is a list of attacks:")
        print("  Replay")
        print("  DOS")
        print("  Targeted App Delete")
        print("  Systematic Brute Force App Delete")
        return userInput()
    
    print(call + " not recognized, please try again for type 'h' for a list of commands")
    userInput()

if __name__ == "__main__":
    main()
