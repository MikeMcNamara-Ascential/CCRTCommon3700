using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Common.Lib.Interfaces;
using Common.Lib.Models;

namespace Common.Lib.Presenters
{
    public class MainFormPresenter<T> where T : IBaseInterface
    {
        protected static IMainFormModel MainFormModel { get; private set; }
        protected T MainFormView { get; private set; }
        
        static MainFormPresenter()
        {
            MainFormModel = new MainFormModel();
        }
        public MainFormPresenter(T view)
        {
            MainFormView = view;
        }
    }
}
