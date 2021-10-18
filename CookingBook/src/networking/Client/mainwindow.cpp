#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_bIsSignInClicked=true;
    sAmountOfIngredients="";
    sUnitsOfMeasurement="";
    m_sWordToFind="";
    sIngredients="";
    m_sFileName="";
    ui->statusbar->showMessage("Connecting...");
    CookingClient.SetClient("25.31.216.94",1234);
    bool bIsConnected=false;
    while(!bIsConnected){
        bIsConnected=CookingClient.TryConnection();
        Sleep(100);
    }
    DrawApplication();
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::DrawApplication()
{
    ui->Menu->setVisible(false);
    ui->frame_recipeAdd->setVisible(false);
    ui->frame_recipesList->setVisible(false);
    ui->frame_RecipeView->setVisible(false);
    ui->password2->setVisible(false);
    ui->line_29->setVisible(false);
    ui->comboBox_categoryView->setEnabled(false);
    ui->comboBox_cuisineView->setEnabled(false);
    ui->lineEdit_NameOfRecipeView->setEnabled(false);
    ui->button_AddToFavourites->setText("Add to favourites");

    QPixmap backgroung_image("D:/CookingClient/Images/background-image.jpg");
    int nWidth=ui->label->width();
    int nHeight=ui->label->height();
    ui->label->setPixmap(backgroung_image.scaled(nWidth,nHeight,Qt::AspectRatioMode()));

    QPixmap menu_icon("D:/CookingClient/Images/menu-icon.jpg");
    QIcon Icon_menu(menu_icon);
    ui->button_menu->setIcon(Icon_menu);
    ui->button_menu->setIconSize(ui->button_menu->size());


    QPixmap icon("D:/CookingClient/Images/meal-white.png");
    nWidth=ui->label_icon->width();
    nHeight=ui->label_icon->height();
    ui->label_icon->setPixmap(icon.scaled(nWidth,nHeight,Qt::KeepAspectRatio));

    QPixmap search_icon("D:/CookingClient/Images/search.png");
    QIcon ButtonIcon(search_icon);
    ui->Search_button->setIcon(ButtonIcon);
    ui->Search_button->setIconSize(ui->Search_button->size());

    QPixmap user_icon("D:/CookingClient/Images/User icon.png");
    nWidth=ui->label_user_icon->width();
    nHeight=ui->label_user_icon->height();
    ui->label_user_icon->setPixmap(user_icon.scaled(nWidth,nHeight,Qt::KeepAspectRatio));

    QPixmap heart_icon("D:/CookingClient/Images/heart.png");
    QIcon Icon_favourites(heart_icon);
    ui->button_favourites->setIcon(Icon_favourites);
    ui->button_favourites->setIconSize(ui->button_favourites->size());

    QPixmap plus_icon("D:/CookingClient/Images/plus.png");
    QIcon Icon_add(plus_icon);
    ui->button_addRecipe->setIcon(Icon_add);
    ui->button_addRecipe->setIconSize(ui->button_addRecipe->size());

//    QPixmap button("D:/CookingClient/Images/background-image.jpg");
//    QIcon Icon(button);
//    ui->pushButton->setIcon(Icon);
//    ui->pushButton->setIconSize(ui->pushButton->size());

    QPixmap snack("D:/CookingClient/Images/snack.jpg");
    QIcon Icon_snack(snack);
    ui->snacksAndsandwiches->setIcon(Icon_snack);
    ui->snacksAndsandwiches->setIconSize(ui->snacksAndsandwiches->size());

    QPixmap salads("D:/CookingClient/Images/salad.jpg");
    QIcon Icon_salad(salads);
    ui->salads->setIcon(Icon_salad);
    ui->salads->setIconSize(ui->salads->size());

    QPixmap first_courses("D:/CookingClient/Images/first courses.jpg");
    QIcon Icon_first_course(first_courses);
    ui->firstCourses->setIcon(Icon_first_course);
    ui->firstCourses->setIconSize(ui->firstCourses->size());

    QPixmap side_dish("D:/CookingClient/Images/side dish.jpg");
    QIcon Icon_side_dish(side_dish);
    ui->sideDishes->setIcon(Icon_side_dish);
    ui->sideDishes->setIconSize(ui->sideDishes->size());

    QPixmap second_courses("D:/CookingClient/Images/second courses.jpg");
    QIcon Icon_second_course(second_courses);
    ui->secondCourses->setIcon(Icon_second_course);
    ui->secondCourses->setIconSize(ui->secondCourses->size());

    QPixmap dessert("D:/CookingClient/Images/dessert2.jpg");
    QIcon Icon_dessert(dessert);
    ui->desserts->setIcon(Icon_dessert);
    ui->desserts->setIconSize(ui->desserts->size());


    QPixmap sauce("D:/CookingClient/Images/sauce.jpg");
    QIcon Icon_sauce(sauce);
    ui->sauces->setIcon(Icon_sauce);
    ui->sauces->setIconSize(ui->sauces->size());

    QPixmap drink("D:/CookingClient/Images/drink.jpg");
    QIcon Icon_drink(drink);
    ui->drinks->setIcon(Icon_drink);
    ui->drinks->setIconSize(ui->drinks->size());

    int i=1;
    ui->Category_buttonGroup->setId(ui->radiobutton_snacks,i);
    ui->Cuisine_buttonGroup->setId(ui->radiobutton_ukrainian,i++);

    ui->Category_buttonGroup->setId(ui->radiobutton_salads,i);
    ui->Cuisine_buttonGroup->setId(ui->radiobutton_american,i++);

    ui->Category_buttonGroup->setId(ui->radiobutton_firstCourses,i);
    ui->Cuisine_buttonGroup->setId(ui->radiobutton_korean,i++);

    ui->Category_buttonGroup->setId(ui->radiobutton_sideDishes,i);
    ui->Cuisine_buttonGroup->setId(ui->radiobutton_mexican,i++);

    ui->Category_buttonGroup->setId(ui->radiobutton_secondCourses,i);
    ui->Cuisine_buttonGroup->setId(ui->radiobutton_italian,i++);

    ui->Category_buttonGroup->setId(ui->radiobutton_desserts,i);
    ui->Cuisine_buttonGroup->setId(ui->radiobutton_french,i++);

    ui->Category_buttonGroup->setId(ui->radiobutton_sauces,i);
    ui->Cuisine_buttonGroup->setId(ui->radiobutton_chinese,i++);

    ui->Category_buttonGroup->setId(ui->radiobutton_drinks,i);
    ui->Cuisine_buttonGroup->setId(ui->radiobutton_indian,i++);

    ui->Cuisine_buttonGroup->setId(ui->radiobutton_spanish,i++);
    ui->Cuisine_buttonGroup->setId(ui->radiobutton_othersCuisines,i++);

    ui->comboBox_category->addItem("Snacks and sandwiches");
    ui->comboBox_category->addItem("Salads");
    ui->comboBox_category->addItem("First courses");
    ui->comboBox_category->addItem("Side dishes");
    ui->comboBox_category->addItem("Second courses");
    ui->comboBox_category->addItem("Desserts");
    ui->comboBox_category->addItem("Sauces");
    ui->comboBox_category->addItem("Drinks");

    ui->comboBox_categoryView->addItem("Snacks and sandwiches");
    ui->comboBox_categoryView->addItem("Salads");
    ui->comboBox_categoryView->addItem("First courses");
    ui->comboBox_categoryView->addItem("Side dishes");
    ui->comboBox_categoryView->addItem("Second courses");
    ui->comboBox_categoryView->addItem("Desserts");
    ui->comboBox_categoryView->addItem("Sauces");
    ui->comboBox_categoryView->addItem("Drinks");

    ui->comboBox_cuisine->addItem("Ukrainian");
    ui->comboBox_cuisine->addItem("American");
    ui->comboBox_cuisine->addItem("Korean");
    ui->comboBox_cuisine->addItem("Mexican");
    ui->comboBox_cuisine->addItem("Italian");
    ui->comboBox_cuisine->addItem("French");
    ui->comboBox_cuisine->addItem("Chinese");
    ui->comboBox_cuisine->addItem("Indian");
    ui->comboBox_cuisine->addItem("Others");

    ui->comboBox_cuisineView->addItem("Ukrainian");
    ui->comboBox_cuisineView->addItem("American");
    ui->comboBox_cuisineView->addItem("Korean");
    ui->comboBox_cuisineView->addItem("Mexican");
    ui->comboBox_cuisineView->addItem("Italian");
    ui->comboBox_cuisineView->addItem("French");
    ui->comboBox_cuisineView->addItem("Chinese");
    ui->comboBox_cuisineView->addItem("Indian");
    ui->comboBox_cuisineView->addItem("Others");



    ui->comboBox_portions->addItem("1");
    ui->comboBox_portions->addItem("2");
    ui->comboBox_portions->addItem("3");
    ui->comboBox_portions->addItem("4");
    ui->comboBox_portions->addItem("5");
    ui->comboBox_portions->addItem("6");
    ui->comboBox_portions->addItem("7");
    ui->comboBox_portions->addItem("8");
    ui->comboBox_portions->addItem("9");
    ui->comboBox_portions->addItem("10");

    ui->comboBox_measurement->addItem("-");
    ui->comboBox_measurement->addItem("kg");
    ui->comboBox_measurement->addItem("l");
    ui->comboBox_measurement->addItem("g");
    ui->comboBox_measurement->addItem("pieces");
    ui->comboBox_measurement->addItem("cups");
    ui->comboBox_measurement->addItem("tablespoon");
    ui->comboBox_measurement->addItem("teaspoon");

    ui->comboBox_measurement_Edit->addItem("-");
    ui->comboBox_measurement_Edit->addItem("kg");
    ui->comboBox_measurement_Edit->addItem("l");
    ui->comboBox_measurement_Edit->addItem("g");
    ui->comboBox_measurement_Edit->addItem("pieces");
    ui->comboBox_measurement_Edit->addItem("cups");
    ui->comboBox_measurement_Edit->addItem("tablespoon");
    ui->comboBox_measurement_Edit->addItem("teaspoon");


}
void MainWindow::on_signin_clicked()
{
    ui->signin->setStyleSheet("background-color: rgb(19,20,50); font: 16pt ""Segoe UI Symbol"";"
                            "color: rgb(184,186,227)");
    ui->signup->setStyleSheet("background-color: rgb(184,186,227);font: 16pt ""Segoe UI Symbol"";"
                            "color: rgb(19,20,50);");
    ui->enter->setText("Sign in");
    ui->password2->setVisible(false);
    ui->line_29->setVisible(false);
    MainWindow::m_bIsSignInClicked=true;

}
void MainWindow::on_signup_clicked()
{
    ui->signup->setStyleSheet("background-color: rgb(19,20,50); font: 16pt ""Segoe UI Symbol"";"
                            "color: rgb(184,186,227)");
    ui->signin->setStyleSheet("background-color: rgb(184,186,227);font: 16pt ""Segoe UI Symbol"";"
                            "color: rgb(19,20,50);");
    ui->enter->setText("Sign up");
    ui->password2->setVisible(true);
    ui->line_29->setVisible(true);
    MainWindow::m_bIsSignInClicked=false;
}
void MainWindow::on_enter_clicked()
{
   QMessageBox qMessage;
    qMessage.setIcon(QMessageBox::Warning);
    if(m_bIsSignInClicked){
        CookingClient.SetClientUser(ui->username->text().toStdString(),ui->password->text().toStdString());

        CookingClient.MakeRequest(std::string("LOGIN"));
        if(CookingClient.LoginUser(CookingClient.GetClientUser())){
            ui->statusbar->showMessage("Login successful");
            ui->GroupBox_SignIn->setVisible(false);
            ui->label_setUsername->setText(QString::fromStdString(CookingClient.GetClientUser().m_sUserName));
        }
        else{
            qMessage.setText("Login or password is not correct");
            qMessage.exec();
        }
    }else{
        if(ui->password->text()==ui->password2->text()){
            CookingClient.SetClientUser(ui->username->text().toStdString(),ui->password->text().toStdString());
            CookingClient.MakeRequest("REGISTER");
            if(CookingClient.RegisterUser(CookingClient.GetClientUser())){
                qMessage.setText("Register successful!");
                qMessage.setIcon(QMessageBox::Information);
                qMessage.exec();
                 ui->GroupBox_SignIn->setVisible(false);
                 ui->label_setUsername->setText(QString::fromStdString(CookingClient.GetClientUser().m_sUserName));
            }else{
                qMessage.setText("Username already exists\nPlease choose another one");
                qMessage.exec();
            }
        }else{
            qMessage.setText("Passwords should be the same");
            qMessage.exec();
        }
    }



}
void MainWindow::on_Search_button_clicked()
{
    QString sSearchQuery=ui->lineEdit_search->text().toLower();
    int nPosOfDelimeter=sSearchQuery.indexOf(" ", Qt::CaseInsensitive);
    if(nPosOfDelimeter<=0){
        if(nPosOfDelimeter==0){
            sSearchQuery.remove(0,1);
        }
        m_sWordToFind=sSearchQuery.toStdString();
        m_szIngredients.push_back(sSearchQuery.toStdString());
    }else{
        sSearchQuery+=" ";
        std::string sTempStr;
        while (nPosOfDelimeter >= 0) {
            if(nPosOfDelimeter==0){
                sSearchQuery.remove(0,1);
                nPosOfDelimeter=sSearchQuery.indexOf(" ", Qt::CaseInsensitive);
                continue;
            }
            sTempStr=sSearchQuery.toStdString();
            sTempStr.erase(nPosOfDelimeter,sTempStr.size()-nPosOfDelimeter);
            m_szIngredients.push_back(sTempStr);
            sSearchQuery.remove(0,nPosOfDelimeter+1);
            sTempStr = "";
            nPosOfDelimeter=sSearchQuery.indexOf(" ", Qt::CaseInsensitive);
         }
    }
    CookingClient.MakeRequest(GET_RECIPES_BY_NAME_REQUEST);
    Sleep(100);
    SRecipe Recipe;
    std::vector<std::string> vec, cat, val, ing;
    CookingClient.GetRecipesByName(m_sWordToFind,m_RecipesNeeded);

    on_results_button_clicked();
}

void MainWindow::on_button_snackAndSandwiches_clicked()
{
    ui->Category_buttonGroup->button(SNACKS_AND_SANDWICHES)->setChecked(true);
    on_results_button_clicked();
}
void MainWindow::on_snacksAndsandwiches_clicked()
{
    ui->Category_buttonGroup->button(SNACKS_AND_SANDWICHES)->setChecked(true);
    on_results_button_clicked();
}
void MainWindow::on_secondCourses_clicked()
{
    ui->Category_buttonGroup->button(SECOND_COURSES)->setChecked(true);
    on_results_button_clicked();
}
void MainWindow::on_button_secondCourses_clicked()
{
    ui->Category_buttonGroup->button(SECOND_COURSES)->setChecked(true);
    on_results_button_clicked();
}
void MainWindow::on_button_salads_clicked()
{
    ui->Category_buttonGroup->button(SALADS)->setChecked(true);
    on_results_button_clicked();
}
void MainWindow::on_salads_clicked()
{
    ui->Category_buttonGroup->button(SALADS)->setChecked(true);
    on_results_button_clicked();
}
void MainWindow::on_button_firstCourses_clicked()
{
    ui->Category_buttonGroup->button(FIRST_COURSES)->setChecked(true);
    on_results_button_clicked();
}
void MainWindow::on_firstCourses_clicked()
{
    ui->Category_buttonGroup->button(FIRST_COURSES)->setChecked(true);
    on_results_button_clicked();
}
void MainWindow::on_button_sideDishes_clicked()
{
    ui->Category_buttonGroup->button(SIDE_DISHES)->setChecked(true);
    on_results_button_clicked();
}
void MainWindow::on_sideDishes_clicked()
{
    ui->Category_buttonGroup->button(SIDE_DISHES)->setChecked(true);
    on_results_button_clicked();
}
void MainWindow::on_button_desserts_clicked()
{
    ui->Category_buttonGroup->button(DESSERTS)->setChecked(true);
    on_results_button_clicked();
}
void MainWindow::on_desserts_clicked()
{
    ui->Category_buttonGroup->button(DESSERTS)->setChecked(true);
    on_results_button_clicked();
}
void MainWindow::on_button_sauces_clicked()
{
    ui->Category_buttonGroup->button(SAUCES)->setChecked(true);
    on_results_button_clicked();
}
void MainWindow::on_sauces_clicked()
{
    ui->Category_buttonGroup->button(SAUCES)->setChecked(true);
    on_results_button_clicked();
}
void MainWindow::on_button_drinks_clicked()
{
    ui->Category_buttonGroup->button(DRINKS)->setChecked(true);
    on_results_button_clicked();
}
void MainWindow::on_drinks_clicked()
{
    ui->Category_buttonGroup->button(DRINKS)->setChecked(true);
    on_results_button_clicked();
}

void MainWindow::on_button_menu_clicked()
{
    if(ui->Menu->isVisible()==true){
        ui->Menu->setVisible(false);
    }else{
        ui->Menu->setVisible(true);
    }
}
std::string MainWindow::GetCategory(int nIndexOfCategory)
{
    std::string sCategory="";
    switch(nIndexOfCategory){
    case 1: sCategory="Snacks and sandwiches";break;
    case 2: sCategory="Salads";break;
    case 3: sCategory="First courses";break;
    case 4: sCategory="Side dishes";break;
    case 5: sCategory="Second courses";break;
    case 6: sCategory="Desserts";break;
    case 7: sCategory="Sauces";break;
    case 8: sCategory="Drinks";break;
    default:break;
    }
    return sCategory;
}
int MainWindow::GetIndexOfCuisine(std::string& sCuisine)
{
   int nCuisine=0;
   if(sCuisine=="Ukrainian"){
       nCuisine=1;
   }else if(sCuisine=="American"){
       nCuisine=2;
   }else if(sCuisine=="Korean"){
       nCuisine=3;
   }else if(sCuisine=="Mexican"){
       nCuisine=4;
   }else if(sCuisine=="Italian"){
       nCuisine=5;
   }else if(sCuisine=="French"){
       nCuisine=6;
   }else if(sCuisine=="Chinese"){
       nCuisine=7;
   }else if(sCuisine=="Indian"){
       nCuisine=8;
   }else if(sCuisine=="Spanish"){
       nCuisine=9;
   }else if(sCuisine=="Others"){
       nCuisine=10;
   }
   return nCuisine;
}
int MainWindow::GetIndexOfCategory(std::string & sCategory)
{
    int nCategory=0;
    if(sCategory=="Snacks and sandwiches"){
        nCategory=1;
    }else if(sCategory=="Salads"){
        nCategory=2;
    }else if(sCategory=="First courses"){
        nCategory=3;
    }else if(sCategory=="Side dishes"){
        nCategory=4;
    }else if(sCategory=="Second course"){
        nCategory=5;
    }else if(sCategory=="Desserts"){
        nCategory=6;
    }else if(sCategory=="Sauces"){
        nCategory=7;
    }else if(sCategory=="Drinks"){
        nCategory=8;
    }
    return nCategory;
}
std::string MainWindow::GetCuisine(int nIndexOfCuisine)
{
    std::string sCuisine="";
    switch(nIndexOfCuisine){
    case 1:sCuisine="Ukrainian";break;
    case 2:sCuisine="American";break;
    case 3:sCuisine="Korean";break;
    case 4:sCuisine="Mexican";break;
    case 5:sCuisine="Italian";break;
    case 6:sCuisine="French";break;
    case 7:sCuisine="Chinese";break;
    case 8:sCuisine="Indian";break;
    case 9:sCuisine="Spanish";break;
    case 10:sCuisine="Others";break;
    default:break;
    }
    return sCuisine;
}
void MainWindow::on_results_button_clicked()
{
    std::vector<std::string> szColumns, szValuesOfColumns, szNames;
    for(int i=1;i<=AMOUNT_OF_CATEGORIES;i++){
        if(ui->Category_buttonGroup->button(i)->isChecked()){
            szColumns.emplace_back("CATEGORY");
            szValuesOfColumns.emplace_back(GetCategory(i));
            ui->Category_buttonGroup->button(i)->setChecked(false);
        }
    }
    for(int i=1;i<=AMOUNT_OF_CUISINES;i++){
        if(ui->Cuisine_buttonGroup->button(i)->isChecked()){
            szColumns.emplace_back("CUISINE");
            szValuesOfColumns.emplace_back(GetCuisine(i));
            ui->Cuisine_buttonGroup->button(i)->setChecked(false);
        }
    }


      CookingClient.MakeRequest(std::string(GET_RECIPES_NEEDED_REQUEST));
        Sleep(100);
      CookingClient.GetRecipesNeeded(m_RecipesNeeded,szColumns,szValuesOfColumns,m_szIngredients);
      Sleep(100);
      if(m_RecipesNeeded.size()){
            ui->frame_recipesList->setVisible(true);

            QStandardItemModel *model = new QStandardItemModel;
            QStandardItem *item;
            for(size_t i=0;i<m_RecipesNeeded.size();i++){
                item = new QStandardItem(QString::fromStdString(m_RecipesNeeded.at(i)));
                item->setTextAlignment(Qt::AlignCenter);
                model->setItem(i,0,item);
            }
            ui->tableView_recipes->setModel(model);
            ui->Menu->setVisible(false);
            m_RecipesNeeded.clear();
      } else{
          QMessageBox::information(this,"Nothing was found", "There are no such recipes in DataBase");
      }

}
void MainWindow::on_button_addRecipe_clicked()
{
    if(ui->frame_recipeAdd->isVisible()){
        ui->frame_recipeAdd->setVisible(false);
    }else{
        ui->frame_recipeAdd->setVisible(true);
    }
}
void MainWindow::on_button_recipeImage_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                        tr("Open File"), QDir::currentPath());
    m_sFileName=fileName.toStdString();
        if (!fileName.isEmpty()) {
            QImage image(fileName);
            if (image.isNull()) {
                QMessageBox::information(this, tr("Image Viewer"),
                                         tr("Cannot load %1.").arg(fileName));
                return;
            }
            //QPixmap recipeImage("D:/CookingClient/Images/drink.jpg");
            QIcon Icon_RecipeImage(QPixmap::fromImage(image));
            ui->button_recipeImage->setText("");
            ui->button_recipeImage->setIcon(Icon_RecipeImage);
            ui->button_recipeImage->setIconSize(ui->button_recipeImage->size());

        }
}
void MainWindow::on_tableView_recipes_clicked(const QModelIndex &index)
{
    szAmountOfIngred.clear();
    szMeasurement.clear();
    szNamesOfIngred.clear();
    std::string sNameOfRecipe=index.data().toString().toStdString();

    CookingClient.MakeRequest(GET_RECIPE_BY_NAME_REQUEST);
    Sleep(100);
    CookingClient.FindRecipeByName(sNameOfRecipe,FoundRecipe);
    Sleep(100);

    if(FoundRecipe.m_sName!=""){
        ui->frame_RecipeView->setVisible(true);
        ui->button_ChangeImage->setVisible(false);
        ui->lineEdit_NameOfRecipeView->setText(QString::fromStdString(FoundRecipe.m_sName));

        sAmountOfIngredients=FoundRecipe.m_sAmountOfIngredients;
        std::cout<<sAmountOfIngredients<<std::endl;
        sUnitsOfMeasurement=FoundRecipe.m_sUnitOfMeasurement;
        sIngredients=FoundRecipe.m_sIngredients;

        std::string sTempStr;

        int nPosOfDelimeter = sAmountOfIngredients.find("/", 0);
        while (nPosOfDelimeter >= 0) {
            sTempStr.insert(0, sAmountOfIngredients, 0, nPosOfDelimeter);
            sAmountOfIngredients.erase(0, nPosOfDelimeter + 1);
            szAmountOfIngred.emplace_back(sTempStr);
            std::cout<<sTempStr<<std::endl;
            sTempStr = "";
            nPosOfDelimeter = sAmountOfIngredients.find("/", 0);
         }
        nPosOfDelimeter = sUnitsOfMeasurement.find("/", 0);
        while (nPosOfDelimeter >= 0) {
            sTempStr.insert(0, sUnitsOfMeasurement, 0, nPosOfDelimeter);
            sUnitsOfMeasurement.erase(0, nPosOfDelimeter + 1);
            szMeasurement.emplace_back(sTempStr);
            std::cout<<sTempStr<<std::endl;
            sTempStr = "";
            nPosOfDelimeter = sUnitsOfMeasurement.find("/", 0);
         }
        nPosOfDelimeter = sIngredients.find("/", 0);
        while (nPosOfDelimeter >= 0) {
            sTempStr.insert(0, sIngredients, 0, nPosOfDelimeter);
            sIngredients.erase(0, nPosOfDelimeter + 1);
            szNamesOfIngred.emplace_back(sTempStr);
            std::cout<<sTempStr<<std::endl;
            sTempStr = "";
            nPosOfDelimeter = sIngredients.find("/", 0);
         }
        for(size_t i=0;i<szAmountOfIngred.size();i++){
            QString ingred=QString::fromStdString(szAmountOfIngred.at(i))+
                     " " + QString::fromStdString(szMeasurement.at(i))+
                     " " + QString::fromStdString(szNamesOfIngred.at(i));
            std::cout<<ingred.toStdString()<<std::endl;
            ui->textEdit_IngredListView->append(ingred);
        }

        ui->comboBox_categoryView->setCurrentText(QString::fromStdString(FoundRecipe.m_sCategory));
        ui->comboBox_cuisineView->setCurrentText(QString::fromStdString(FoundRecipe.m_sCuisine));
        ui->lineEdit_portionsView->setText(QString::number(FoundRecipe.m_nAmountOfPortions));
        ui->lineEdit_caloriesView->setText(QString::number(FoundRecipe.m_dCalories));
        ui->lineEdit_weightView->setText(QString::number(FoundRecipe.m_dWeigh));
        ui->textEdit_cookingAlgorithmView->setText(QString::fromStdString(FoundRecipe.m_sCookingAlgorithm));

        CookingClient.MakeRequest(FILE_REQUEST);
        Sleep(100);
        if(CookingClient.RecvFile("D:\\CookingClient\\Images\\",sNameOfRecipe+".jpg")){

        QString img=QString::fromStdString(sNameOfRecipe)+".jpg";
        QImage image(img);
        bool bIsImageNull=true;
        while(bIsImageNull){
                bIsImageNull=image.isNull();
        }
//        if (image.isNull()) {
//            QMessageBox::information(this, tr("Image Viewer"),
//                                     tr("Cannot load %1.").arg(img));
//            return;
//        }
        //QPixmap recipeImage("D:/CookingClient/Images/drink.jpg");
        QIcon Icon_RecipeImage(QPixmap::fromImage(image));
        ui->button_recipeImageView->setText("");
        ui->button_recipeImageView->setIcon(Icon_RecipeImage);
        ui->button_recipeImageView->setIconSize(ui->button_recipeImageView->size());
        }
        if(FoundRecipe.m_nIdOfOwner==CookingClient.GetClientUser().m_nID){
            ui->button_EditRecipe->setEnabled(true);
            ui->button_Save->setEnabled(true);
            ui->button_EditCancel->setEnabled(true);
            ui->button_AddToFavourites->setText("Delete from favourites");
        }else if(CookingClient.GetClientUser().m_sFavorites!="" &&
                 CookingClient.GetClientUser().m_sFavorites.find(std::to_string(FoundRecipe.m_nId),0)>=0){
            ui->button_AddToFavourites->setText("Delete from favourites");

        }else {
            ui->button_AddToFavourites->setText("Add to favourites");
        }

    } else {
        QMessageBox::information(this,"Nothing was found", "There are no such recipes in DataBase");
    }


    ui->frame_recipesList->setVisible(false);





}
void MainWindow::on_button_AddOK_clicked()
{
    QMessageBox AddRecipeMessageBox;
    SRecipe RecipeForAdding;
    RecipeForAdding.m_sName=ui->lineEdit_NameOfRecipe->text().toLower().toStdString();
    RecipeForAdding.m_sCategory=ui->comboBox_category->currentText().toStdString();
    RecipeForAdding.m_sCuisine=ui->comboBox_cuisine->currentText().toStdString();
    RecipeForAdding.m_sAmountOfIngredients=sAmountOfIngredients;
    sAmountOfIngredients="";
    RecipeForAdding.m_sUnitOfMeasurement=sUnitsOfMeasurement;
    sUnitsOfMeasurement="";
    RecipeForAdding.m_sIngredients=sIngredients;
    sIngredients="";
    RecipeForAdding.m_sCookingAlgorithm=ui->textEdit_cookingAlgorithm->toPlainText().toStdString();
    RecipeForAdding.m_nAmountOfPortions=ui->comboBox_portions->currentText().toInt();
    RecipeForAdding.m_dWeigh=ui->lineEdit_weight->text().toDouble();
    RecipeForAdding.m_dCalories=ui->lineEdit_calories->text().toDouble();
    RecipeForAdding.m_nIdOfOwner=CookingClient.GetClientUser().m_nID;
    if(CookingClient.GetClientUser().m_nID==1){
        RecipeForAdding.m_bIsAccessible=true;
    }else{
        RecipeForAdding.m_bIsAccessible=false;

    }

    CookingClient.MakeRequest(std::string(ADD_RECIPE_REQUEST));
    Sleep(100);
    if(CookingClient.AddRecipeToServer(RecipeForAdding)){
        Sleep(100);
        CookingClient.MakeRequest(SEND_FILE_REQUEST);
        Sleep(100);
        CookingClient.SendFile(m_sFileName,RecipeForAdding.m_sName+".jpg");
        AddRecipeMessageBox.setText("Recipe """+QString::fromStdString(RecipeForAdding.m_sName)+""" was added");
        AddRecipeMessageBox.setIcon(QMessageBox::Information);
        CookingClient.MakeRequest(GET_RECIPE_BY_NAME_REQUEST);
        Sleep(100);
        CookingClient.FindRecipeByName(RecipeForAdding.m_sName,FoundRecipe);
        Sleep(100);
        CookingClient.MakeRequest(ADD_RECIPE_TO_FAVOURITES_REQUEST);
        Sleep(100);
        CookingClient.AddRecipeToFavorites(FoundRecipe.m_nId);

    }
    else{
        AddRecipeMessageBox.setText("Recipe """+QString::fromStdString(RecipeForAdding.m_sName)+""" was not added");
         AddRecipeMessageBox.setIcon(QMessageBox::Warning);
    }
    AddRecipeMessageBox.exec();
    m_sFileName="";
    on_button_AddCancel_clicked();
}
void MainWindow::on_add_ingred_clicked()
{
    sAmountOfIngredients+=ui->lineEdit_amountOfIngred->text().toStdString()+"/";
    sUnitsOfMeasurement+=ui->comboBox_measurement->currentText().toStdString()+"/";
    sIngredients+=ui->lineEdit_nameOfIngred->text().toStdString()+"/";
    QString sIngred=ui->lineEdit_amountOfIngred->text()+
            " "+ui->comboBox_measurement->currentText()+
            " "+ui->lineEdit_nameOfIngred->text();
    ui->textEdit_IngredList->append(sIngred);

    ui->lineEdit_amountOfIngred->clear();
    ui->lineEdit_nameOfIngred->clear();
}

void MainWindow::on_button_AddCancel_clicked()
{
    ui->button_recipeImage->setText("dowload image...");
    ui->button_recipeImage->setIcon(QIcon());
    ui->lineEdit_NameOfRecipe->clear();
    ui->lineEdit_amountOfIngred->clear();
    ui->lineEdit_nameOfIngred->clear();
    ui->textEdit_IngredList->clear();
    ui->textEdit_cookingAlgorithm->clear();
    ui->lineEdit_weight->clear();
    ui->lineEdit_calories->clear();

    ui->frame_recipeAdd->setVisible(false);
}
void MainWindow::on_pushButton_close_clicked()
{
    ui->frame_RecipeView->setVisible(false);
    szAmountOfIngred.clear();
    szMeasurement.clear();
    szNamesOfIngred.clear();
    ui->textEdit_IngredListView->clear();
}
void MainWindow::on_lineEdit_portionsView_editingFinished()
{
    ui->textEdit_IngredListView->clear();
    for(size_t i=0;i<szAmountOfIngred.size();i++){
        szAmountOfIngred.at(i)=std::to_string(std::atof(szAmountOfIngred.at(i).c_str())/FoundRecipe.m_nAmountOfPortions
                                *ui->lineEdit_portionsView->text().toInt());
        szAmountOfIngred.at(i).erase(szAmountOfIngred.at(i).find(".",0)+2,szAmountOfIngred.at(i).size()
                                     -szAmountOfIngred.at(i).find(".",0)-2);
        QString ingred=QString::fromStdString(szAmountOfIngred.at(i))+
                 " " + QString::fromStdString(szMeasurement.at(i))+
                 " " + QString::fromStdString(szNamesOfIngred.at(i));
        ui->textEdit_IngredListView->append(ingred);
    }
    ui->lineEdit_caloriesView->setText(QString::number(FoundRecipe.m_dCalories/FoundRecipe.m_nAmountOfPortions
                                       *ui->lineEdit_portionsView->text().toInt()));
    ui->lineEdit_weightView->setText(QString::number(FoundRecipe.m_dWeigh/FoundRecipe.m_nAmountOfPortions
                                                     *ui->lineEdit_portionsView->text().toInt()));
}
void MainWindow::on_button_EditRecipe_clicked()
{
    ui->lineEdit_amountOfIngred_Edit->setEnabled(true);
    ui->comboBox_measurement_Edit->setEnabled(true);
    ui->lineEdit_nameOfIngred_Edit->setEnabled(true);
    ui->add_ingred_Edit->setEnabled(true);
    ui->textEdit_IngredListView->clear();

    ui->comboBox_categoryView->setEnabled(true);
    ui->comboBox_cuisineView->setEnabled(true);
    //ui->lineEdit_portionsView->setEnabled(true);
    ui->lineEdit_portionsView->setReadOnly(false);
    ui->lineEdit_caloriesView->setEnabled(true);
    ui->lineEdit_caloriesView->setReadOnly(false);
    ui->lineEdit_weightView->setEnabled(true);
    ui->lineEdit_weightView->setReadOnly(false);


    ui->textEdit_cookingAlgorithmView->setEnabled(true);
    ui->textEdit_cookingAlgorithmView->setReadOnly(false);
    ui->button_ChangeImage->setVisible(true);


}
void MainWindow::on_button_Save_clicked()
{
    ui->lineEdit_amountOfIngred_Edit->setEnabled(false);
    ui->comboBox_measurement_Edit->setEnabled(false);
    ui->lineEdit_nameOfIngred_Edit->setEnabled(false);
    ui->button_AddOK->setEnabled(false);

    ui->comboBox_categoryView->setEnabled(false);
    ui->comboBox_cuisineView->setEnabled(false);
    //ui->lineEdit_portionsView->setEnabled(false);
    ui->lineEdit_caloriesView->setEnabled(false);
    ui->lineEdit_weightView->setEnabled(false);

    ui->textEdit_cookingAlgorithmView->setEnabled(false);
    ui->button_ChangeImage->setVisible(false);

    QMessageBox AddRecipeMessageBox;
    FoundRecipe.m_sName=ui->lineEdit_NameOfRecipeView->text().toLower().toStdString();
    FoundRecipe.m_sCategory=ui->comboBox_categoryView->currentText().toStdString();
    FoundRecipe.m_sCuisine=ui->comboBox_cuisineView->currentText().toStdString();
    FoundRecipe.m_sAmountOfIngredients=sAmountOfIngredients;
    sAmountOfIngredients="";
    FoundRecipe.m_sUnitOfMeasurement=sUnitsOfMeasurement;
    sUnitsOfMeasurement="";
    FoundRecipe.m_sIngredients=sIngredients;
    sIngredients="";
    FoundRecipe.m_sCookingAlgorithm=ui->textEdit_cookingAlgorithmView->toPlainText().toStdString();
    FoundRecipe.m_nAmountOfPortions=ui->lineEdit_portionsView->text().toInt();
    FoundRecipe.m_dWeigh=ui->lineEdit_weightView->text().toDouble();
    FoundRecipe.m_dCalories=ui->lineEdit_caloriesView->text().toDouble();
    FoundRecipe.m_nIdOfOwner=CookingClient.GetClientUser().m_nID;
    if(CookingClient.GetClientUser().m_nID==1){
        FoundRecipe.m_bIsAccessible=true;
    }else{
        FoundRecipe.m_bIsAccessible=false;
    }

    CookingClient.MakeRequest(CHANGE_RECIPE_REQUEST);
    Sleep(100);
    if(CookingClient.ChangeRecipe(FoundRecipe)){
        Sleep(100);
        CookingClient.MakeRequest(SEND_FILE_REQUEST);
        Sleep(100);
        CookingClient.SendFile(m_sFileName,FoundRecipe.m_sName+".jpg");
        AddRecipeMessageBox.setText("Changes were saved");
        AddRecipeMessageBox.setIcon(QMessageBox::Information);

    }
    else{
        AddRecipeMessageBox.setText("Changes were not saved");
         AddRecipeMessageBox.setIcon(QMessageBox::Warning);
    }
    AddRecipeMessageBox.exec();
    m_sFileName="";

}
void MainWindow::on_button_ChangeImage_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                        tr("Open File"), QDir::currentPath());
    m_sFileName=fileName.toStdString();
        if (!fileName.isEmpty()) {
            QImage image(fileName);
            if (image.isNull()) {
                QMessageBox::information(this, tr("Image Viewer"),
                                         tr("Cannot load %1.").arg(fileName));
                return;
            }
            //QPixmap recipeImage("D:/CookingClient/Images/drink.jpg");
            QIcon Icon_RecipeImage(QPixmap::fromImage(image));
            ui->button_recipeImageView->setText("");
            ui->button_recipeImageView->setIcon(Icon_RecipeImage);
            ui->button_recipeImageView->setIconSize(ui->button_recipeImageView->size());

        }
}
void MainWindow::on_button_EditCancel_clicked()
{
    szAmountOfIngred.clear();
    szMeasurement.clear();
    szNamesOfIngred.clear();
    ui->textEdit_IngredListView->clear();
    ui->lineEdit_NameOfRecipeView->setText(QString::fromStdString(FoundRecipe.m_sName));
    ui->button_ChangeImage->setVisible(false);

    sAmountOfIngredients=FoundRecipe.m_sAmountOfIngredients;
    std::cout<<sAmountOfIngredients<<std::endl;
    sUnitsOfMeasurement=FoundRecipe.m_sUnitOfMeasurement;
    sIngredients=FoundRecipe.m_sIngredients;

    std::string sTempStr;

    int nPosOfDelimeter = sAmountOfIngredients.find("/", 0);
    while (nPosOfDelimeter >= 0) {
        sTempStr.insert(0, sAmountOfIngredients, 0, nPosOfDelimeter);
        sAmountOfIngredients.erase(0, nPosOfDelimeter + 1);
        szAmountOfIngred.emplace_back(sTempStr);
        std::cout<<sTempStr<<std::endl;
        sTempStr = "";
        nPosOfDelimeter = sAmountOfIngredients.find("/", 0);
     }
    nPosOfDelimeter = sUnitsOfMeasurement.find("/", 0);
    while (nPosOfDelimeter >= 0) {
        sTempStr.insert(0, sUnitsOfMeasurement, 0, nPosOfDelimeter);
        sUnitsOfMeasurement.erase(0, nPosOfDelimeter + 1);
        szMeasurement.emplace_back(sTempStr);
        std::cout<<sTempStr<<std::endl;
        sTempStr = "";
        nPosOfDelimeter = sUnitsOfMeasurement.find("/", 0);
     }
    nPosOfDelimeter = sIngredients.find("/", 0);
    while (nPosOfDelimeter >= 0) {
        sTempStr.insert(0, sIngredients, 0, nPosOfDelimeter);
        sIngredients.erase(0, nPosOfDelimeter + 1);
        szNamesOfIngred.emplace_back(sTempStr);
        std::cout<<sTempStr<<std::endl;
        sTempStr = "";
        nPosOfDelimeter = sIngredients.find("/", 0);
     }
    for(size_t i=0;i<szAmountOfIngred.size();i++){
        QString ingred=QString::fromStdString(szAmountOfIngred.at(i))+
                 " " + QString::fromStdString(szMeasurement.at(i))+
                 " " + QString::fromStdString(szNamesOfIngred.at(i));
        std::cout<<ingred.toStdString()<<std::endl;
        ui->textEdit_IngredListView->append(ingred);
    }

    ui->comboBox_categoryView->setCurrentText(QString::fromStdString(FoundRecipe.m_sCategory));
    ui->comboBox_cuisineView->setCurrentText(QString::fromStdString(FoundRecipe.m_sCuisine));
    ui->lineEdit_portionsView->setText(QString::number(FoundRecipe.m_nAmountOfPortions));
    ui->lineEdit_caloriesView->setText(QString::number(FoundRecipe.m_dCalories));
    ui->lineEdit_weightView->setText(QString::number(FoundRecipe.m_dWeigh));
    ui->textEdit_cookingAlgorithmView->setText(QString::fromStdString(FoundRecipe.m_sCookingAlgorithm));
}
void MainWindow::on_add_ingred_Edit_clicked()
{
    sAmountOfIngredients+=ui->lineEdit_amountOfIngred_Edit->text().toStdString()+"/";
    sUnitsOfMeasurement+=ui->comboBox_measurement_Edit->currentText().toStdString()+"/";
    sIngredients+=ui->lineEdit_nameOfIngred_Edit->text().toStdString()+"/";
    QString sIngred=ui->lineEdit_amountOfIngred_Edit->text()+
            " "+ui->comboBox_measurement_Edit->currentText()+
            " "+ui->lineEdit_nameOfIngred_Edit->text();
    ui->textEdit_IngredListView->append(sIngred);

    ui->lineEdit_amountOfIngred_Edit->clear();
    ui->lineEdit_nameOfIngred_Edit->clear();
}
void MainWindow::on_button_favourites_clicked()
{
    CookingClient.MakeRequest(GET_FAVOURITE_RECIPES_REQUEST);
    Sleep(100);
    CookingClient.GetFavouriteRecipes(m_RecipesNeeded);

    ui->frame_recipesList->setVisible(true);
    QStandardItemModel *model = new QStandardItemModel;
    QStandardItem *item;
    for(size_t i=0;i<m_RecipesNeeded.size();i++){
        item = new QStandardItem(QString::fromStdString(m_RecipesNeeded.at(i)));
        item->setTextAlignment(Qt::AlignCenter);
        model->setItem(i,0,item);
    }
    ui->tableView_recipes->setModel(model);
    ui->Menu->setVisible(false);
    m_RecipesNeeded.clear();
}
void MainWindow::on_button_AddToFavourites_clicked()
{
    if(ui->button_AddToFavourites->text()=="Delete from favourites"){
        if(FoundRecipe.m_nIdOfOwner==CookingClient.GetClientUser().m_nID){
            QMessageBox message;
//            message.warning(this,"Deleting from favourites",
//                            "Do you want to delete the recipe from favourites? "
//                            "You won't be able to restore it");
            message.setText("Do you want to delete the recipe from favourites? "
                            "You won't be able to restore it.");
            message.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
            message.exec();
            if(message.clickedButton()==message.button(QMessageBox::Yes)){
                CookingClient.MakeRequest(DELETE_FROM_FAVOURITES_REQUEST);
                Sleep(100);
                CookingClient.DeleteRecipeFromFavourites(FoundRecipe.m_nId);
                Sleep(100);
                CookingClient.MakeRequest(DELETE_RECIPE_REQUEST);
                Sleep(100);
                CookingClient.DeleteRecipe(FoundRecipe.m_nId);
                on_pushButton_close_clicked();
            }
        }else{
            CookingClient.MakeRequest(DELETE_FROM_FAVOURITES_REQUEST);
            Sleep(100);
            CookingClient.DeleteRecipeFromFavourites(FoundRecipe.m_nId);
            ui->button_AddToFavourites->setText("Add to favourites");
        }
    }else if(ui->button_AddToFavourites->text()=="Add to favourites"){
        CookingClient.MakeRequest(ADD_RECIPE_TO_FAVOURITES_REQUEST);
        Sleep(100);
        CookingClient.AddRecipeToFavorites(FoundRecipe.m_nId);
        ui->button_AddToFavourites->setText("Delete from favourites");
    }
}


void MainWindow::on_pushButton_closeList_clicked()
{
    ui->frame_recipesList->setVisible(false);
}

