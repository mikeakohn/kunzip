

default:
	@+make -C build

clean:
	@rm -f build/*.o kunzip.exe kunzip
	@rm -f examples/csharp/TestKunzip.exe


