import sys
class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

total = 0
total_tests = 0

RED = '\x1b[6;30;41m'
GREEN = '\x1b[6;30;42m'
WHITE = '\x1b[0m'
import subprocess as sp
proc = sp.run(["ninja", "-C", "../builddir"])
def test_suite(test):
    global total
    global total_tests
    proc_execute = sp.run(["../builddir/42sh", "-c", test], capture_output=True, text=True)

    temp_stdout = (proc_execute.stdout)
    temp_returncode = (proc_execute.returncode)

    proc_realsh = sp.run(test, shell=True, capture_output=True, text=True)

    real_stdout = (proc_realsh.stdout)
    real_returncode = (proc_realsh.returncode)

    if (temp_stdout != real_stdout) or (temp_returncode != real_returncode):
        print( bcolors.WARNING + 'ERROR ON : '+ test + '\x1b[0m')
        print("valeur de retour obtenue: ", temp_returncode)
        print("\nvaleur de retour attendue : ", real_returncode)
        print("valeur obtenue: \n\n", temp_stdout)
        print("valeur réele: \n\n", real_stdout)
        total_tests += 1

    else:
        print(bcolors.OKCYAN + 'VICTORY ROYALE ON : ' + test, '\x1b[0m')
        print("valeur de retour obtenue : ", temp_returncode)
        print("\nvaleur de retour attendue : ", real_returncode)

        print("\nvaleur obtenue: \n\n" + temp_stdout)
        print("valeur attendue: \n\n" + real_stdout)
        total_tests += 1
        total += 1

#tests are here
test_suite("echo ok")
test_suite("echo -n 'salut les nuls'")
#test_suite('echo -e "je mapelle \nhenry"')
#test_suite("echo -n -e 'je suis vraime\nnt très con'")
#test_suite("echo -n -n -n -n -n -e -e 'jeaimevraient les cons\nejkm'")
#test_suite("echo j'aimerai; echo bien; echo reussir; echo ma vie")
test_suite("echo etre aimeeeee;echo pop;")
test_suite("echo -mfjdklmsqjfklmqs 'coucou chef'")
test_suite("echo lol je suis con;");
test_suite("echo -a -e -b -tasd yop");
test_suite("echo --eyop");
test_suite("echo \\d\\g\\h\\i\\j\\k\\l\\m");
test_suite("ls")
test_suite("ls .")
test_suite("ls ttjkeltt")
test_suite("ls ../src/builtin")

test_suite("cat Vendredi")
test_suite("Bonsoir Paris")

test_suite("if echo ok; then echo ko; fi")
test_suite("if ls o zaman; then echo suuuu; fi")
test_suite("if ls Vanguard; then echo bien; else echo BATTLEFIELD; fi")
test_suite("if ls toulouk; then echo dadada; else if echo baby shark; then echo dodododo; fi; fi")
test_suite("if echo vac; then if echo tiger; then ls no; fi; fi")

test_suite("echo hallo | tr a e")

test_suite("until echo ok; do ls; done")
test_suite("until ls; do echo ok; done")
test_suite("until echo ok; do echo ko; done")
test_suite("while echo without_do; echo where_is_do; done")
test_suite("while echo without_done; do echo where_is_done")
test_suite("while echo without_do_done; echo where_are_do_done")
test_suite("until echo without_do; echo where_is_do; done")
test_suite("until echo without_done; do echo where_is_done")
test_suite("until echo without_do_done; echo where_are_do_done")
test_suite("while echo without_do; echo where_is_do; done; echo it_will_stop; ls")
test_suite("until echo without_done; do echo where_is_done; echo it_will_stop; ls")
test_suite("until; do echo without_condition; done")
test_suite("until ls; do echo without_second_separator done")
test_suite("until echo without_separators do ls done")
test_suite("until echo without_body; do; done")
test_suite("until")
test_suite("while; do echo without_condition; done")
test_suite("while ls; do echo without_second_separator done")
test_suite("while echo without_separators do ls done")
test_suite("while")
test_suite("while echo without_body; do; done")

test_suite("cd ../src; ls")
test_suite("cd .; ls")
test_suite("cd ../src/builtin; ls; cd ..; cd ..; cd tests; ls")
test_suite("cd ..; ls; cd src; ls; cd builtin; ls")
test_suite("cd ../src/builtin; ls")
test_suite("cd; ls")
test_suite("cd unexisting_dir")
test_suite("cd unexisting_dir; ls; echo it will not stop")
test_suite("cd unexistind_dir; echo it will not stop; cd")
test_suite("cd; cd unexisting_dir")
test_suite("cd; cd unexisting_dir; ls")
test_suite("cd; ls; cd 42sh; ls; cd src/builtin; ls; cd ../../builddir; ls; cd; cd 42sh/tests; ls; echo test ok")
test_suite("cd unexisting_dir; ls; cd 42sh; ls; cd src/builtin; ls; cd ../../builddir; ls; cd; cd 42sh/tests; ls; echo test ok")
test_suite("cd; ls; cd 42sh; ls; cd src/builtin; ls; cd ../../builddir; ls; cd; cd 42sh/unexist")

test_suite("if echo sava; then ls dfhfds; fi")
test_suite("echo vos meres || echo les deleguees")
test_suite("echo vos meres && echo les deleguees")
test_suite(" ls || echo les deleguees")
test_suite("echo vos meres && ls")
test_suite("ls fdhdsj && echo david La farge")
test_suite("ls fdhdsj || echo david La farge")
test_suite("if echo david la && echo farge pokemon; then ls ../src/; fi")
test_suite("if echo david la || echo farge pokemon; then echo poule mouiller; fi")
test_suite("if echo david la && echo farge pokemon; then ls hdjgjhs; fi")

#test_suite("! echo lo")
#test_suite("echo hallo | tr a i | tr i e")
#endof tests

if (total == total_tests):
    print(GREEN + "REUSSI")
    print(GREEN + "tests réussis: ", total)
    print(GREEN + "nombres de tests: ", total_tests, WHITE)
else:
    print(RED + "RATE")
    print(RED + "tests réussis: ", total)
    print(RED + "nombres de tests: ", total_tests, WHITE)
