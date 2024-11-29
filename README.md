The structure of the table is as illustrated below

![image](https://github.com/user-attachments/assets/cf963924-2972-41eb-a2c5-fae41380f9a9)


DESCRIPTION:
- the MainWindow Ui contains a LineEdit (search_le) a TableView (display_tv) and a PushButton (commit_btn)
- when a valid VIN (vehicle ID number) is inserted inside the LineEdit and return is pressed, the value provided filter the table, displaing only the record that has the corrisponding VIN
- the Commit_btn (when pressed) try to change one value on the last column with model->setData() and after call model->submitAll to fetch the data to the DB
