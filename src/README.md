## Part 1. Настройка gitlab-runner

**== Задание ==**

##### Поднять виртуальную машину *Ubuntu Server 20.04 LTS*

##### Скачать и установить на виртуальную машину **gitlab-runner**

##### Запустить **gitlab-runner** и зарегистрировать его для использования в текущем проекте (*DO6_CICD*)

**== Решение ==**

1) Устанавливаем gitlab-runner:

    - `curl -L "https://packages.gitlab.com/install/repositories/runner/gitlab-runner/script.deb.sh" | sudo bash`

    - `apt-cache madison gitlab-runner`

    - `sudo apt-get install gitlab-runner=14.9.1`

2) Устанавливаем все необходимые пакеты в Ubuntu:

    - `sudo apt install make`

    - `sudo apt install gcc`

    - `sudo apt install clang-format`

3) Регистрируем проект:

    - `sudo gitlab-runner register`, указываем URL, токен и тэг

![gitlab-runner register](images/part%201/register.png)

## Part 2. Сборка

**== Задание ==**

#### Написать этап для **CI** по сборке приложений из проекта *C2_SimpleBashUtils*:

##### В файле _gitlab-ci.yml_ добавить этап запуска сборки через мейк файл из проекта _C2_

##### Файлы, полученные после сборки (артефакты), сохранять в произвольную директорию со сроком хранения 30 дней.

**== Решение ==**

1) Этап по сборке приложений представлен в .gitlab-ci.yml `stage: build`:

![.gitlab-ci.yml file content](images/part%202/build_stage.png)

2) Запускаем на Ubuntu gitlab-runner командой `gitlab-runner run`:

![gitlab-runner run](images/part%202/gitlab_runner_run.png)

3) После того, как проект будет запушен, в разделе `CI/CD` -> `Pipelines` появится информация о результате:

    - Сборка прошла успешно:

![build stage successfully passed](images/part%202/build_stage_passed.png)

    - Ошибка при сборке:

![build stage successfully passed](images/part%202/build_stage_failure.png)

## Part 3. Тест кодстайла

**== Задание ==**

#### Написать этап для **CI**, который запускает скрипт кодстайла (*clang-format*):

##### Если кодстайл не прошел, то "зафейлить" пайплайн

##### В пайплайне отобразить вывод утилиты *clang-format*

**== Решение ==**

1) Напишем скрипт `style_test.sh`, который будет проверять кодстайл. В случае, если будет нарушение кодстайла, выводится текст с ошибкой и скрипт завершается с кодом 1. В этом случае пайплайн будет зафейлен:

![style_test.sh content](images/part%203/style_test_script.png)

2) Дополним файл `.gitlab-ci.yml` новым этапом `check-style`:

![.gitlab-ci.yml new stage check-style](images/part%203/gitlab_ci_style.png)

3) Результат пайплайна:

    - При нарушении кодстайла будет следующий вывод:

![clang failure](images/part%203/style_test_failure.png)

    - При соответствии кодстайлу:

![clang success](images/part%203/style_test_success.png)

### Part 4. Интеграционные тесты

**== Задание ==**

#### Написать этап для **CI**, который запускает ваши интеграционные тесты из того же проекта:

##### Запускать этот этап автоматически только при условии, если сборка и тест кодстайла прошли успешно

##### Если тесты не прошли, то "зафейлить" пайплайн

##### В пайплайне отобразить вывод, что интеграционные тесты успешно прошли / провалились

**== Решение ==**

1) Напишем скрипты `src/integration_tests/cat_test.sh` и `src/integration_tests/grep_test.sh` для проверки проекта и создадим текстовый шаблон для `src/integration_tests/test.txt`.

2) Скрипт `src/integration_test.sh` запускает написанные нами скрипты и в случае ошибки возвращает код 1.

![integration_test.sh content](images/part%204/integration_test_content.png)

3) Дополним файл `.gitlab-ci.yml` новым этапом `check-style`:

![.gitlab-ci.yml new stage integration](images/part%204/integration_stage_added.png)

3) Результаты пайплайна:

    - При успешной проверке:

![gitlab-runner success 1](images/part%204/success_stage_1.png)

![gitlab-runner success 2](images/part%204/success_stage_2.png)

    - При провале теста cat:

![gitlab-runner failure 1](images/part%204/failure_stage_1.png)

![gitlab-runner failure 2](images/part%204/failure_stage_2.png)

    - В случае, если предыдущие тесты провалены:

![gitlab-runner failure 3](images/part%204/failure_stage_3.png)

![gitlab-runner failure 4](images/part%204/failure_stage_4.png)

### Part 5. Этап деплоя

**== Задание ==**

##### Поднять вторую виртуальную машину *Ubuntu Server 20.04 LTS*

#### Написать этап для **CD**, который "разворачивает" проект на другой виртуальной машине:

##### Запускать этот этап вручную при условии, что все предыдущие этапы прошли успешно

##### Написать bash-скрипт, который при помощи **ssh** и **scp** копирует файлы, полученные после сборки (артефакты), в директорию */usr/local/bin* второй виртуальной машины

##### В файле _gitlab-ci.yml_ добавить этап запуска написанного скрипта

##### В случае ошибки "зафейлить" пайплайн

**== Решение ==**

1) Подготовка к выполнению задания:

    - После создания второй машины устанавливаем на ней владельцем дирректории **/usr/local/bin** пользователя *fossowan* командой: `chown fossowan /usr/local/bin` .

    - На машине с **gitlab-runner** переключаемся на пользователя gitlab-runner `sudo su gitlab-runner`, создаем ssh-ключ `ssh-keygen` и устанавливаем беспарольное SSH копированием открытого ключа с помощью команды `ssh-copy-id fossowan@192.168.100.10` .

    - Устанавливаем статическое соединение между машинами.

2) Создаем скрипт с копированием файлов через `scp`:

![deployment script](images/part%205/deployment_script.png)

3) Добавляем новый этап в *.gitlab-ci.yml*:

![deployment stage](images/part%205/deployment_stage.png)

4) При успешном пайплайне этапов до делпоя в гитлабе пайплайн будет отображаться следующим образом:

![pipeline deployment stage](images/part%205/deployment_in_pipeline.png)

5) При успешном пайплайне:

![success stage](images/part%205/success_stage.png)

![new files on the second machine](images/part%205/new_files.png)

6) При проваленном пайплайне работа приостановятся и файлы не скопируются:

![skipped stage](images/part%205/skipped_stage.png)

![no files when skipped](images/part%205/no_files_when_skipped.png)

### Part 6. Дополнительно. Уведомления

**== Задание ==**

##### Настроить уведомления о успешном/неуспешном выполнении пайплайна через бота с именем "[ваш nickname] DO6 CI/CD" в *Telegram*

- Текст уведомления должен содержать информацию об успешности прохождения как этапа **CI**, так и этапа **CD**.
- В остальном текст уведомления может быть произвольным.

**== Решение ==**

1) Создаем бота в Телеграм с помощью [BotFather](https://t.me/BotFather):

![creating bot with BotFather](images/part%206/bot_father.png)

2) Создаем скрипт *notify.sh*, со следующим содержание:

![notify script content](images/part%206/notify_script.png)

3) Добавляем в *.gitlab-ci.yml* новый этап и дополняем этап деплоя. Финальный файл выглядит следующим образом:

![final .gitlab-ci.yml file content](images/part%206/final_yml_file_content.png)

Пример сообщений:

![messages about pipeline in telegram](images/part%206/tg_messages.png)