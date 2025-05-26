package furhatos.app.newskill.flow.partials

import furhatos.app.newskill.data.locale.Localization
import furhatos.app.newskill.flow.utils.changeOutputLanguage
import furhatos.app.newskill.nlu.ChangeLanguage
import furhatos.app.newskill.nlu.Language
import furhatos.flow.kotlin.furhat
import furhatos.flow.kotlin.onResponse
import furhatos.flow.kotlin.partialState

val ChangeLocale =
    partialState {
        onResponse<ChangeLanguage> {
            changeOutputLanguage(it.language)
            println(it.language)
            furhat.say(Localization.getLocalizedString("detected_language_change", it.language))
            reentry()
        }

        onResponse<Language> {
            changeOutputLanguage(it.language)
            println(it.language)
            furhat.say(Localization.getLocalizedString("detected_language_change", it.language))
            reentry()
        }
    }
