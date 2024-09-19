```console
$ sudo docker build . --network=host -t ps2024
$ sudo docker run ps2024 /work/hello_with_lifegame
# You will see lifegame here.
$ sudo docker run ps2024 /work/hello_with_whitespace
Hello, world! from whitespace!
Hello, World!
```
