#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "Client.h"
#include <QMainWindow>
#include<QPixmap>
#include <QMessageBox>
#include<QFileDialog>
#include "QStandardItemModel"
#include "QStandardItem"

#define AMOUNT_OF_CATEGORIES 8
#define AMOUNT_OF_CUISINES 10




QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();    
private slots:
    void on_signin_clicked();

    void on_signup_clicked();

    void on_enter_clicked();

    void DrawApplication();

    void on_Search_button_clicked();

    void on_button_snackAndSandwiches_clicked();

    void on_snacksAndsandwiches_clicked();

    void on_secondCourses_clicked();

    void on_button_secondCourses_clicked();

    void on_button_menu_clicked();

    void on_results_button_clicked();

    std::string GetCategory(int nIndexOfCategory);

    std::string GetCuisine(int nIndexOfCuisine);

    int GetIndexOfCategory(std::string & sCategory);

    int GetIndexOfCuisine(std::string& sCuisine);

    void on_button_salads_clicked();

    void on_salads_clicked();

    void on_button_firstCourses_clicked();

    void on_firstCourses_clicked();

    void on_button_sideDishes_clicked();

    void on_sideDishes_clicked();

    void on_button_desserts_clicked();

    void on_desserts_clicked();

    void on_button_sauces_clicked();

    void on_sauces_clicked();

    void on_button_drinks_clicked();

    void on_drinks_clicked();

    void on_button_addRecipe_clicked();

    void on_button_recipeImage_clicked();

    void on_tableView_recipes_clicked(const QModelIndex &index);

    void on_button_AddOK_clicked();

    void on_add_ingred_clicked();

    void on_button_AddCancel_clicked();

    void on_pushButton_close_clicked();

    void on_lineEdit_portionsView_editingFinished();

    void on_button_EditRecipe_clicked();

    void on_button_Save_clicked();

    void on_button_ChangeImage_clicked();

    void on_button_EditCancel_clicked();

    void on_add_ingred_Edit_clicked();

    void on_button_favourites_clicked();

    void on_button_AddToFavourites_clicked();

    void on_pushButton_closeList_clicked();

private:
    std::vector<std::string> m_szIngredients;
    std::vector<std::string> m_RecipesNeeded;
    std::vector<std::string> szAmountOfIngred, szMeasurement,szNamesOfIngred;
    std::string sAmountOfIngredients;
    std::string sUnitsOfMeasurement;
    std::string sIngredients;
    std::string m_sWordToFind;
    ECategories m_Category;
    bool m_bIsSignInClicked;
    Ui::MainWindow *ui;
    Client CookingClient;
    std::string m_sFileName;
    SRecipe FoundRecipe;
};
#endif // MAINWINDOW_H
