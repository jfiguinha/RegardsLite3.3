#include <header.h>
#include "FolderRefreshService.h"
#include "CentralWindow.h"
#include "SqlFindPhotos.h"
#include <ThumbnailBuffer.h>
#include "ViewerParam.h"
#include "ViewerParamInit.h"
#include <window_id.h>
#include <TreeWindow.h>
#include <ScrollbarWnd.h>
using namespace Regards::Viewer;
using namespace Regards::Sqlite;

FolderRefreshService::FolderRefreshService(CCentralWindow* centralWnd,
                                           wxWindow*       eventSink,
                                           int             faceDetection)
    : centralWnd(centralWnd)
    , eventSink(eventSink)
    , faceDetection(faceDetection)
{}

// ── Fichier courant ──────────────────────────────────────────────────────────

void FolderRefreshService::SetFirstFileToShow(const wxString& path)
{
    firstFileToShow = path;
}

void FolderRefreshService::SetLocalFilename(const wxString& path)
{
    localFilename = path;
}

wxString FolderRefreshService::GetLocalFilename() const
{
    return localFilename;
}

// ── Navigation dans le buffer ────────────────────────────────────────────────

bool FolderRefreshService::FindNextValidFile(const wxString& filename)
{
    bool isFound = false;
    wxString lastFile = centralWnd->ImageFin(false);
    localFilename = filename;

    do
    {
        isFound = CThumbnailBuffer::FindValidFile(localFilename);
        if (!isFound)
        {
            if (lastFile == localFilename)
                break;
            localFilename = centralWnd->ImageSuivante(false);
        }
    } while (!isFound);

    return isFound;
}

bool FolderRefreshService::FindNextValidFile()
{
    return FindNextValidFile(localFilename);
}

bool FolderRefreshService::FindPreviousValidFile()
{
    bool isFound = false;
    wxString firstFile = centralWnd->ImageDebut(false);

    do
    {
        isFound = CThumbnailBuffer::FindValidFile(localFilename);
        if (!isFound)
        {
            if (firstFile == localFilename)
                break;
            localFilename = centralWnd->ImagePrecedente(false);
        }
    } while (!isFound);

    return isFound;
}

void FolderRefreshService::ResolveCurrentFilename()
{
    const int pictureCount = CThumbnailBuffer::GetVectorSize();

    if (pictureCount == 0)
    {
        localFilename.clear();
        return;
    }

    bool found = false;

    if (!localFilename.empty())
    {
        found = FindNextValidFile(localFilename);
        if (!found)
            found = FindPreviousValidFile();
    }

    if (!found)
        localFilename = CThumbnailBuffer::GetVectorValue(0).GetPath();
}

// ── Diff de liste ────────────────────────────────────────────────────────────

bool FolderRefreshService::HasPictureListChanged(const PhotosVector* newPictures) const
{
    const auto oldPictures = CThumbnailBuffer::GetVectorList();

    if (oldPictures == nullptr)
        return true;

    if (newPictures == nullptr || newPictures->size() != oldPictures->size())
        return true;

    return !std::equal(
        newPictures->begin(), newPictures->end(),
        oldPictures->begin(),
        [](const CPhotos& lhs, const CPhotos& rhs)
        {
            return lhs.path == rhs.path;
        });
}


// ── Point d'entrée principal ─────────────────────────────────────────────────

void FolderRefreshService::UpdateFolderStatic(bool isDeleteFolder, bool refreshPhotos)
{
    wxBusyCursor busy;

    CSqlFindPhotos sqlFindPhotos;

    bool isSqlUpdate = false;

    //------------------------------------------
    // Mode d'affichage
    //------------------------------------------
    int typeAffichage = 0;
    if (auto* config = CMainParamInit::getInstance(); config != nullptr)
        typeAffichage = config->GetTypeAffichage();

    //------------------------------------------
    // Chargement des photos filtrées
    //------------------------------------------
    auto* pictures = new PhotosVector();
    sqlFindPhotos.GetAllPhotos(pictures);

    //------------------------------------------
    // Fichier courant
    //------------------------------------------
    localFilename = !firstFileToShow.empty()
                    ? firstFileToShow
                    : centralWnd->GetFilename();

    //------------------------------------------
    // Abandon si la liste n'a pas changé
    //------------------------------------------
    if (!HasPictureListChanged(pictures))
    {
        delete pictures;
        return;
    }

    //------------------------------------------
    // Mise à jour du buffer de miniatures
    //------------------------------------------
    CThumbnailBuffer::InitVectorList(pictures);

    //------------------------------------------
    // Résolution du fichier sélectionné
    //------------------------------------------
    ResolveCurrentFilename();

    //------------------------------------------
    // Rafraîchissement de la fenêtre centrale
    //------------------------------------------
    centralWnd->SetListeFile(
        localFilename,
        isDeleteFolder || refreshPhotos,
        isSqlUpdate,
        typeAffichage);

    //------------------------------------------
    // Remise à zéro de l'état transitoire
    //------------------------------------------
    firstFileToShow.clear();
    init = true;

}
