# Smart Room System

Development Guide:
	1. Setup Toolchain: 
		http://esp-idf.readthedocs.io/en/latest/get-started/
		
	2. Make:
		A. linux:
			1. Export xtensa-esp32-elf/bin directory
				export PATH="$PATH:$HOME/esp/xtensa-esp32-elf/bin"

				f.e.: export PATH="$PATH:$HOME/esp/xtensa-esp32-elf/bin"
					
				
			2. Export IDF_PATH directory:
				export IDF_PATH="~/esp/esp-idf"
				
				f.e.: export IDF_PATH=~/esp/esp-idf
			
			ad1. ad2. Quick:
				export PATH="$PATH:$HOME/esp/xtensa-esp32-elf/bin" && export IDF_PATH="~/esp/esp-idf"
			
			4. Go to project directory
			3. Execute command:
				a. To configure: 
					make menuconfig
				b. To flash: 
					make flash
				c. To watch:
				 	make monitor
				d. To flash and watch: 
					make flash monitor
